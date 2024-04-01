import { HidNpadButton } from "@nx.js/constants";
import { Page } from "../renderer/page";
import { drawText } from "../renderer/text";
import { event } from "../types/event";
import { MainPage } from "./main";
import { router } from "../router/router";

export class SecondPage extends Page {
  protected events: event[] = [
    {
      event: "buttondown",
      callback: (event) => {
        if (event.detail & HidNpadButton.B) {
          router.go("/");
        }
      },
    },
  ];

  protected _renderTitle(): void {
    super._renderTitle();

    // draw a border showing the title area
    this.ctx.beginPath();
    this.ctx.rect(
      this.titleArea.x,
      this.titleArea.y,
      this.titleArea.x2,
      this.titleArea.y2
    );
    this.ctx.strokeStyle = "blue";
    this.ctx.closePath();
    this.ctx.stroke();
  }

  protected _renderBody(): void {
    super._renderBody();

    this.ctx.beginPath();
    this.ctx.rect(
      this.bodyArea.x,
      this.bodyArea.y,
      this.bodyArea.x2,
      this.bodyArea.y2
    );
    this.ctx.strokeStyle = "red";
    this.ctx.closePath();
    this.ctx.stroke();

    drawText("This is the second page!", this.bodyArea.x, this.bodyArea.y + 20);
  }
}
