
export function drawTitle(title: string) {
    const ctx = screen.getContext('2d');
    ctx.fillStyle = 'white';
    ctx.font = '42px system-ui';
    ctx.textAlign = 'center';
    ctx.textBaseline = 'bottom';


    ctx.fillText(title, screen.width/2, 50);

    ctx.save()
}

export function drawList(items: string[], startX: number, startY: number, incrementX: number = 0, incrementY: number = 15) {
    const ctx = screen.getContext('2d');
    ctx.fillStyle = 'white';
    ctx.font = '12px system-ui';


    for(var [index, item] of items.entries()) {
        ctx.fillText(item, startX + (incrementX * index), startY + (incrementY * index));
    }



    ctx.save()
}