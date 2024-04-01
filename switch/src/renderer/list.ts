import { drawText } from "./text";

export function drawTextList(
  items: string[],
  startX: number,
  startY: number,
  increment: number,
  direction: "vertical" | "horizontal" = "vertical",
  textSize: number = 12
) {
  for (var [index, item] of items.entries()) {
    const x = direction == "vertical" ? startX : startX + increment * index;
    const y = direction == "horizontal" ? startY : startY + increment * index;
    drawText(item, x, y, textSize);
  }
}
