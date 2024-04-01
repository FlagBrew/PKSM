import { event } from "../types/event";
import { Rectangle } from "../types/rectangle";
import { drawText } from "./text";

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

  protected events: event[] = [];

  title: string;
  ctx: CanvasRenderingContext2D;

  constructor(title: string) {
    this.title = title;

    this.ctx = screen.getContext("2d");
  }

  protected _renderTitle() {
    // Clear the title area
    this.ctx.clearRect(
      this.titleArea.x,
      this.titleArea.y,
      this.titleArea.x2,
      this.titleArea.y2
    );

    drawText(
      this.title,
      screen.availWidth / 2,
      this.titleArea.y2 / 2,
      42,
      "top",
      "center"
    );
  }

  protected _renderBody() {
    // Clear the body area
    this.ctx.clearRect(
      this.bodyArea.x,
      this.bodyArea.y,
      this.bodyArea.x2,
      this.bodyArea.y2
    );

    // Body will be rendered by the class extending this abstract class.
  }

  private _registerEventListeners() {
    for (var event of this.events) {
      addEventListener(event.event, event.callback);
    }
  }

  private _destroyEventListeners() {
    for (var event of this.events) {
      removeEventListener(event.event, event.callback);
    }
  }

  _unload() {
    this._destroyEventListeners();
  }

  render() {
    this._renderTitle();
    this._renderBody();
    this._registerEventListeners();
  }
}
