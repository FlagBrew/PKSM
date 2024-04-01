export function drawText(
  text: string,
  x: number,
  y: number,
  fontSize: number = 12
) {
  const ctx = screen.getContext("2d");
  ctx.beginPath();
  ctx.fillStyle = "white";
  ctx.textBaseline = "bottom";
  ctx.textAlign = "start";
  ctx.font = `${fontSize}px system-ui`;

  ctx.fillText(text, x, y);
  ctx.closePath();
}
