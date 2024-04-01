// import { drawList, drawText, drawTitle } from "./utils/text";

import { MainPage } from "./pages/main";
import { SecondPage } from "./pages/second";
import { HidNpadButton } from "@nx.js/constants";
import { router } from "./router/router";

router.load();

// drawTitle("Main Screen");

// function listApplications() {
//     let items = []
//     for (const app of Switch.Application) {
//         items.push(app.name);
//     }

//     drawList(items, 150, 125);
// }

// listApplications();

// addEventListener('buttondown', (event) => {
//     drawText(`Pushed button ${event.which}`, 300, 300);
// })

// const page = new MainPage("Main Page");
// // const page2 = new SecondPage("Second Page");

// page.render();

// addEventListener("buttondown", (event) => {
//   if (event.detail & HidNpadButton.A) {
//     page2.render();
//   } else if (event.detail & HidNpadButton.B) {
//     page1.render();
//   }
// });
