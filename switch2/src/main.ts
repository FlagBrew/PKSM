import { drawList, drawTitle } from "./utils/text";

drawTitle("Main Screen");


function listApplications() {
    let items = []
    for (const app of Switch.Application) {
        items.push(app.name);
    }

    drawList(items, 150, 125);
}


listApplications();
