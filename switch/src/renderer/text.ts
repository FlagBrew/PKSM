export function drawText(
  text: string,
  x: number,
  y: number,
  fontSize: number = 12,
  baseline: CanvasTextBaseline = "bottom",
  textAlign: CanvasTextAlign = "start"
) {
  const ctx = screen.getContext("2d");
  ctx.beginPath();
  ctx.fillStyle = "white";
  ctx.textBaseline = baseline;
  ctx.textAlign = textAlign;
  ctx.font = `${fontSize}px system-ui`;

  ctx.fillText(text, x, y);
  ctx.closePath();
}
