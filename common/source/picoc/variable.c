/* picoc variable storage. This provides ways of defining and accessing
 * variables */
 
#include "interpreter.h"

/* maximum size of a value to temporarily copy while we create a variable */
#define MAX_TMP_COPY_BUF 256


/* initialise the variable system */
void VariableInit(Picoc *pc)
{
    TableInitTable(&(pc->GlobalTable), &(pc->GlobalHashTable)[0], GLOBAL_TABLE_SIZE, TRUE);
    TableInitTable(&pc->StringLiteralTable, &pc->StringLiteralHashTable[0], STRING_LITERAL_TABLE_SIZE, TRUE);
    pc->TopStackFrame = NULL;
}

/* deallocate the contents of a variable */
void VariableFree(Picoc *pc, struct Value *Val)
{
    if (Val->ValOnHeap || Val->AnyValOnHeap)
    {
        /* free function bodies */
        if (Val->Typ == &pc->FunctionType && Val->Val->FuncDef.Intrinsic == NULL && Val->Val->FuncDef.Body.Pos != NULL)
            HeapFreeMem(pc, (void *)Val->Val->FuncDef.Body.Pos);

        /* free macro bodies */
        if (Val->Typ == &pc->MacroType)
            HeapFreeMem(pc, (void *)Val->Val->MacroDef.Body.Pos);

        /* free the AnyValue */
        if (Val->AnyValOnHeap)
            HeapFreeMem(pc, Val->Val);
    }

    /* free the value */
    if (Val->ValOnHeap)
        HeapFreeMem(pc, Val);
}

/* deallocate the global table and the string literal table */
void VariableTableCleanup(Picoc *pc, struct Table *HashTable)
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    int Count;
    
    for (Count = 0; Count < HashTable->Size; Count++)
    {
        for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            VariableFree(pc, Entry->p.v.Val);
                
            /* free the hash table entry */
            HeapFreeMem(pc, Entry);
        }
    }
}

void VariableCleanup(Picoc *pc)
{
    VariableTableCleanup(pc, &pc->GlobalTable);
    VariableTableCleanup(pc, &pc->StringLiteralTable);
}

/* allocate some memory, either on the heap or the stack and check if we've run out */
void *VariableAlloc(Picoc *pc, struct ParseState *Parser, int Size, int OnHeap)
{
    void *NewValue;
    
    if (OnHeap)
        NewValue = HeapAllocMem(pc, Size);
    else
        NewValue = HeapAllocStack(pc, Size);
    
    if (NewValue == NULL)
        ProgramFail(Parser, "out of memory");
    
#ifdef DEBUG_HEAP
    if (!OnHeap)
        printf("pushing %d at 0x%lx\n", Size, (unsigned long)NewValue);
#endif
        
    return NewValue;
}

/* allocate a value either on the heap or the stack using space dependent on what type we want */
struct Value *VariableAllocValueAndData(Picoc *pc, struct ParseState *Parser, int DataSize, int IsLValue, struct Value *LValueFrom, int OnHeap)
{
    struct Value *NewValue = VariableAlloc(pc, Parser, MEM_ALIGN(sizeof(struct Value)) + DataSize, OnHeap);
    NewValue->Val = (union AnyValue *)((char *)NewValue + MEM_ALIGN(sizeof(struct Value)));
    NewValue->ValOnHeap = OnHeap;
    NewValue->AnyValOnHeap = FALSE;
    NewValue->ValOnStack = !OnHeap;
    NewValue->IsLValue = IsLValue;
    NewValue->LValueFrom = LValueFrom;
    if (Parser) 
        NewValue->ScopeID = Parser->ScopeID;

    NewValue->OutOfScope = 0;
    
    return NewValue;
}

/* allocate a value given its type */
struct Value *VariableAllocValueFromType(Picoc *pc, struct ParseState *Parser, struct ValueType *Typ, int IsLValue, struct Value *LValueFrom, int OnHeap)
{
    int Size = TypeSize(Typ, Typ->ArraySize, FALSE);
    struct Value *NewValue = VariableAllocValueAndData(pc, Parser, Size, IsLValue, LValueFrom, OnHeap);
    assert(Size >= 0 || Typ == &pc->VoidType);
    NewValue->Typ = Typ;
    
    return NewValue;
}

/* allocate a value either on the heap or the stack and copy its value. handles overlapping data */
struct Value *VariableAllocValueAndCopy(Picoc *pc, struct ParseState *Parser, struct Value *FromValue, int OnHeap)
{
    struct ValueType *DType = FromValue->Typ;
    struct Value *NewValue;
    char TmpBuf[MAX_TMP_COPY_BUF];
    int CopySize = TypeSizeValue(FromValue, TRUE);

    assert(CopySize <= MAX_TMP_COPY_BUF);
    memcpy((void *)&TmpBuf[0], (void *)FromValue->Val, CopySize);
    NewValue = VariableAllocValueAndData(pc, Parser, CopySize, FromValue->IsLValue, FromValue->LValueFrom, OnHeap);
    NewValue->Typ = DType;
    memcpy((void *)NewValue->Val, (void *)&TmpBuf[0], CopySize);
    
    return NewValue;
}

/* allocate a value either on the heap or the stack from an existing AnyValue and type */
struct Value *VariableAllocValueFromExistingData(struct ParseState *Parser, struct ValueType *Typ, union AnyValue *FromValue, int IsLValue, struct Value *LValueFrom)
{
    struct Value *NewValue = VariableAlloc(Parser->pc, Parser, sizeof(struct Value), FALSE);
    NewValue->Typ = Typ;
    NewValue->Val = FromValue;
    NewValue->ValOnHeap = FALSE;
    NewValue->AnyValOnHeap = FALSE;
    NewValue->ValOnStack = FALSE;
    NewValue->IsLValue = IsLValue;
    NewValue->LValueFrom = LValueFrom;
    
    return NewValue;
}

/* allocate a value either on the heap or the stack from an existing Value, sharing the value */
struct Value *VariableAllocValueShared(struct ParseState *Parser, struct Value *FromValue)
{
    return VariableAllocValueFromExistingData(Parser, FromValue->Typ, FromValue->Val, FromValue->IsLValue, FromValue->IsLValue ? FromValue : NULL);
}

/* reallocate a variable so its data has a new size */
void VariableRealloc(struct ParseState *Parser, struct Value *FromValue, int NewSize)
{
    if (FromValue->AnyValOnHeap)
        HeapFreeMem(Parser->pc, FromValue->Val);
        
    FromValue->Val = VariableAlloc(Parser->pc, Parser, NewSize, TRUE);
    FromValue->AnyValOnHeap = TRUE;
}

int VariableScopeBegin(struct ParseState * Parser, int* OldScopeID)
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    Picoc * pc = Parser->pc;
    int Count;
    #ifdef VAR_SCOPE_DEBUG
    int FirstPrint = 0;
    #endif
    
    struct Table * HashTable = (pc->TopStackFrame == NULL) ? &(pc->GlobalTable) : &(pc->TopStackFrame)->LocalTable;

    if (Parser->ScopeID == -1) return -1;

    /* XXX dumb hash, let's hope for no collisions... */
    *OldScopeID = Parser->ScopeID;
    Parser->ScopeID = (int)(intptr_t)(Parser->SourceText) * ((int)(intptr_t)(Parser->Pos) / sizeof(char*));
    /* or maybe a more human-readable hash for debugging? */
    /* Parser->ScopeID = Parser->Line * 0x10000 + Parser->CharacterPos; */
    
    for (Count = 0; Count < HashTable->Size; Count++)
    {
        for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            if (Entry->p.v.Val->ScopeID == Parser->ScopeID && Entry->p.v.Val->OutOfScope)
            {
                Entry->p.v.Val->OutOfScope = FALSE;
                Entry->p.v.Key = (char*)((intptr_t)Entry->p.v.Key & ~1);
                #ifdef VAR_SCOPE_DEBUG
                if (!FirstPrint) { PRINT_SOURCE_POS; }
                FirstPrint = 1;
                printf(">>> back into scope: %s %x %d\n", Entry->p.v.Key, Entry->p.v.Val->ScopeID, Entry->p.v.Val->Val->Integer);
                #endif
            }
        }
    }

    return Parser->ScopeID;
}

void VariableScopeEnd(struct ParseState * Parser, int ScopeID, int PrevScopeID)
{
    struct TableEntry *Entry;
    struct TableEntry *NextEntry;
    Picoc * pc = Parser->pc;
    int Count;
    #ifdef VAR_SCOPE_DEBUG
    int FirstPrint = 0;
    #endif

    struct Table * HashTable = (pc->TopStackFrame == NULL) ? &(pc->GlobalTable) : &(pc->TopStackFrame)->LocalTable;

    if (ScopeID == -1) return;

    for (Count = 0; Count < HashTable->Size; Count++)
    {
        for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = NextEntry)
        {
            NextEntry = Entry->Next;
            if (Entry->p.v.Val->ScopeID == ScopeID && !Entry->p.v.Val->OutOfScope)
            {
                #ifdef VAR_SCOPE_DEBUG
                if (!FirstPrint) { PRINT_SOURCE_POS; }
                FirstPrint = 1;
                printf(">>> out of scope: %s %x %d\n", Entry->p.v.Key, Entry->p.v.Val->ScopeID, Entry->p.v.Val->Val->Integer);
                #endif
                Entry->p.v.Val->OutOfScope = TRUE;
                Entry->p.v.Key = (char*)((intptr_t)Entry->p.v.Key | 1); /* alter the key so it won't be found by normal searches */
            }
        }
    }

    Parser->ScopeID = PrevScopeID;
}

int VariableDefinedAndOutOfScope(Picoc * pc, const char* Ident)
{
    struct TableEntry *Entry;
    int Count;

    struct Table * HashTable = (pc->TopStackFrame == NULL) ? &(pc->GlobalTable) : &(pc->TopStackFrame)->LocalTable;
    for (Count = 0; Count < HashTable->Size; Count++)
    {
        for (Entry = HashTable->HashTable[Count]; Entry != NULL; Entry = Entry->Next)
        {
            if (Entry->p.v.Val->OutOfScope && (char*)((intptr_t)Entry->p.v.Key & ~1) == Ident)
                return TRUE;
        }
    }
    return FALSE;
}

/* define a variable. Ident must be registered */
struct Value *VariableDefine(Picoc *pc, struct ParseState *Parser, char *Ident, struct Value *InitValue, struct ValueType *Typ, int MakeWritable)
{
    struct Value * AssignValue;
    struct Table * currentTable = (pc->TopStackFrame == NULL) ? &(pc->GlobalTable) : &(pc->TopStackFrame)->LocalTable;
    
    int ScopeID = Parser ? Parser->ScopeID : -1;
#ifdef VAR_SCOPE_DEBUG
    if (Parser) fprintf(stderr, "def %s %x (%s:%d:%d)\n", Ident, ScopeID, Parser->FileName, Parser->Line, Parser->CharacterPos);
#endif
    
    if (InitValue != NULL)
        AssignValue = VariableAllocValueAndCopy(pc, Parser, InitValue, pc->TopStackFrame == NULL);
    else
        AssignValue = VariableAllocValueFromType(pc, Parser, Typ, MakeWritable, NULL, pc->TopStackFrame == NULL);
    
    AssignValue->IsLValue = MakeWritable;
    AssignValue->ScopeID = ScopeID;
    AssignValue->OutOfScope = FALSE;

    if (!TableSet(pc, currentTable, Ident, AssignValue, Parser ? ((char *)Parser->FileName) : NULL, Parser ? Parser->Line : 0, Parser ? Parser->CharacterPos : 0))
        ProgramFail(Parser, "'%s' is already defined", Ident);
    
    return AssignValue;
}

/* define a variable. Ident must be registered. If it's a redefinition from the same declaration don't throw an error */
struct Value *VariableDefineButIgnoreIdentical(struct ParseState *Parser, char *Ident, struct ValueType *Typ, int IsStatic, int *FirstVisit)
{
    Picoc *pc = Parser->pc;
    struct Value *ExistingValue;
    const char *DeclFileName;
    int DeclLine;
    int DeclColumn;
    
    /* is the type a forward declaration? */
    if (TypeIsForwardDeclared(Parser, Typ))
        ProgramFail(Parser, "type '%t' isn't defined", Typ);

    if (IsStatic)
    {
        char MangledName[LINEBUFFER_MAX];
        char *MNPos = &MangledName[0];
        char *MNEnd = &MangledName[LINEBUFFER_MAX-1];
        const char *RegisteredMangledName;
        
        /* make the mangled static name (avoiding using sprintf() to minimise library impact) */
        memset((void *)&MangledName, '\0', sizeof(MangledName));
        *MNPos++ = '/';
        strncpy(MNPos, (char *)Parser->FileName, MNEnd - MNPos);
        MNPos += strlen(MNPos);
        
        if (pc->TopStackFrame != NULL)
        {
            /* we're inside a function */
            if (MNEnd - MNPos > 0) *MNPos++ = '/';
            strncpy(MNPos, (char *)pc->TopStackFrame->FuncName, MNEnd - MNPos);
            MNPos += strlen(MNPos);
        }
            
        if (MNEnd - MNPos > 0) *MNPos++ = '/';
        strncpy(MNPos, Ident, MNEnd - MNPos);
        RegisteredMangledName = TableStrRegister(pc, MangledName);
        
        /* is this static already defined? */
        if (!TableGet(&pc->GlobalTable, RegisteredMangledName, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn))
        {
            /* define the mangled-named static variable store in the global scope */
            ExistingValue = VariableAllocValueFromType(Parser->pc, Parser, Typ, TRUE, NULL, TRUE);
            TableSet(pc, &pc->GlobalTable, (char *)RegisteredMangledName, ExistingValue, (char *)Parser->FileName, Parser->Line, Parser->CharacterPos);
            *FirstVisit = TRUE;
        }

        /* static variable exists in the global scope - now make a mirroring variable in our own scope with the short name */
        VariableDefinePlatformVar(Parser->pc, Parser, Ident, ExistingValue->Typ, ExistingValue->Val, TRUE);
        return ExistingValue;
    }
    else
    {
        if (Parser->Line != 0 && TableGet((pc->TopStackFrame == NULL) ? &pc->GlobalTable : &pc->TopStackFrame->LocalTable, Ident, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn)
                && DeclFileName == Parser->FileName && DeclLine == Parser->Line && DeclColumn == Parser->CharacterPos)
            return ExistingValue;
        else
            return VariableDefine(Parser->pc, Parser, Ident, NULL, Typ, TRUE);
    }
}

/* check if a variable with a given name is defined. Ident must be registered */
int VariableDefined(Picoc *pc, const char *Ident)
{
    struct Value *FoundValue;
    
    if (pc->TopStackFrame == NULL || !TableGet(&pc->TopStackFrame->LocalTable, Ident, &FoundValue, NULL, NULL, NULL))
    {
        if (!TableGet(&pc->GlobalTable, Ident, &FoundValue, NULL, NULL, NULL))
            return FALSE;
    }

    return TRUE;
}

/* get the value of a variable. must be defined. Ident must be registered */
void VariableGet(Picoc *pc, struct ParseState *Parser, const char *Ident, struct Value **LVal)
{
    if (pc->TopStackFrame == NULL || !TableGet(&pc->TopStackFrame->LocalTable, Ident, LVal, NULL, NULL, NULL))
    {
        if (!TableGet(&pc->GlobalTable, Ident, LVal, NULL, NULL, NULL))
        {
            if (VariableDefinedAndOutOfScope(pc, Ident))
                ProgramFail(Parser, "'%s' is out of scope", Ident);
            else
                ProgramFail(Parser, "'%s' is undefined", Ident);
        }
    }
}

/* define a global variable shared with a platform global. Ident will be registered */
void VariableDefinePlatformVar(Picoc *pc, struct ParseState *Parser, char *Ident, struct ValueType *Typ, union AnyValue *FromValue, int IsWritable)
{
    struct Value *SomeValue = VariableAllocValueAndData(pc, NULL, 0, IsWritable, NULL, TRUE);
    SomeValue->Typ = Typ;
    SomeValue->Val = FromValue;
    
    if (!TableSet(pc, (pc->TopStackFrame == NULL) ? &pc->GlobalTable : &pc->TopStackFrame->LocalTable, TableStrRegister(pc, Ident), SomeValue, Parser ? Parser->FileName : NULL, Parser ? Parser->Line : 0, Parser ? Parser->CharacterPos : 0))
        ProgramFail(Parser, "'%s' is already defined", Ident);
}

/* free and/or pop the top value off the stack. Var must be the top value on the stack! */
void VariableStackPop(struct ParseState *Parser, struct Value *Var)
{
    int Success;
    
#ifdef DEBUG_HEAP
    if (Var->ValOnStack)
        printf("popping %ld at 0x%lx\n", (unsigned long)(sizeof(struct Value) + TypeSizeValue(Var, FALSE)), (unsigned long)Var);
#endif
        
    if (Var->ValOnHeap)
    { 
        if (Var->Val != NULL)
            HeapFreeMem(Parser->pc, Var->Val);
            
        Success = HeapPopStack(Parser->pc, Var, sizeof(struct Value));                       /* free from heap */
    }
    else if (Var->ValOnStack)
        Success = HeapPopStack(Parser->pc, Var, sizeof(struct Value) + TypeSizeValue(Var, FALSE));  /* free from stack */
    else
        Success = HeapPopStack(Parser->pc, Var, sizeof(struct Value));                       /* value isn't our problem */
        
    if (!Success)
        ProgramFail(Parser, "stack underrun");
}

/* add a stack frame when doing a function call */
void VariableStackFrameAdd(struct ParseState *Parser, const char *FuncName, int NumParams)
{
    struct StackFrame *NewFrame;
    
    HeapPushStackFrame(Parser->pc);
    NewFrame = HeapAllocStack(Parser->pc, sizeof(struct StackFrame) + sizeof(struct Value *) * NumParams);
    if (NewFrame == NULL)
        ProgramFail(Parser, "out of memory");
        
    ParserCopy(&NewFrame->ReturnParser, Parser);
    NewFrame->FuncName = FuncName;
    NewFrame->Parameter = (NumParams > 0) ? ((void *)((char *)NewFrame + sizeof(struct StackFrame))) : NULL;
    TableInitTable(&NewFrame->LocalTable, &NewFrame->LocalHashTable[0], LOCAL_TABLE_SIZE, FALSE);
    NewFrame->PreviousStackFrame = Parser->pc->TopStackFrame;
    Parser->pc->TopStackFrame = NewFrame;
}

/* remove a stack frame */
void VariableStackFramePop(struct ParseState *Parser)
{
    if (Parser->pc->TopStackFrame == NULL)
        ProgramFail(Parser, "stack is empty - can't go back");
        
    ParserCopy(Parser, &Parser->pc->TopStackFrame->ReturnParser);
    Parser->pc->TopStackFrame = Parser->pc->TopStackFrame->PreviousStackFrame;
    HeapPopStackFrame(Parser->pc);
}

/* get a string literal. assumes that Ident is already registered. NULL if not found */
struct Value *VariableStringLiteralGet(Picoc *pc, char *Ident)
{
    struct Value *LVal = NULL;

    if (TableGet(&pc->StringLiteralTable, Ident, &LVal, NULL, NULL, NULL))
        return LVal;
    else
        return NULL;
}

/* define a string literal. assumes that Ident is already registered */
void VariableStringLiteralDefine(Picoc *pc, char *Ident, struct Value *Val)
{
    TableSet(pc, &pc->StringLiteralTable, Ident, Val, NULL, 0, 0);
}

/* check a pointer for validity and dereference it for use */
void *VariableDereferencePointer(struct ParseState *Parser, struct Value *PointerValue, struct Value **DerefVal, int *DerefOffset, struct ValueType **DerefType, int *DerefIsLValue)
{
    if (DerefVal != NULL)
        *DerefVal = NULL;
        
    if (DerefType != NULL)
        *DerefType = PointerValue->Typ->FromType;
        
    if (DerefOffset != NULL)
        *DerefOffset = 0;
        
    if (DerefIsLValue != NULL)
        *DerefIsLValue = TRUE;

    return PointerValue->Val->Pointer;
}

