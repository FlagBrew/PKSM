export type event = {
  event:
    | "buttondown"
    | "buttonup"
    | "error"
    | "keydown"
    | "keyup"
    | "unhandledrejection"
    | "unload";
  callback: EventListenerOrEventListenerObject<any> | null;
};
