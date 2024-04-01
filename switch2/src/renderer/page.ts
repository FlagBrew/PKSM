import { Rectangle } from "../types/rectangle";

export abstract class Page {
  /**
   * Area Definitions
   *
   * Defines the safe render zone for each area
   * Currently tested only in handheld mode, will investigate TVs later.
   *
   */
  protected titleArea: Rectangle = {
    x: 0,
    x2: screen.availWidth,
    y: 0,
    y2: 100,
  };
  protected bodyArea: Rectangle = {
    x: 0,
    x2: screen.availWidth,
    y: 120,
    y2: screen.availHeight - 120,
  };

  title: string;
  ctx: CanvasRenderingContext2D;

  constructor(title: string) {
    this.title = title;

    this.ctx = screen.getContext("2d");
  }

  protected _renderTitle() {
    this.ctx.fillStyle = "white";
    this.ctx.font = "42px system-ui";
    this.ctx.textAlign = "center";
    this.ctx.textBaseline = "top";

    // Clear the title area
    this.ctx.clearRect(
      this.titleArea.x,
      this.titleArea.y,
      this.titleArea.x2,
      this.titleArea.y2
    );
    this.ctx.fillText(this.title, screen.availWidth / 2, this.titleArea.y2 / 2);
  }

  protected _renderBody() {
    // Clear the body area
    this.ctx.clearRect(
      this.bodyArea.x,
      this.bodyArea.y,
      this.bodyArea.x2,
      this.bodyArea.y2
    );
  }

  render() {
    this._renderTitle();
    this._renderBody();
  }
}
