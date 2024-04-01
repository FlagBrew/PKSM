import { MainPage } from "../pages/main";
import { SecondPage } from "../pages/second";
import { route } from "../types/route";

export class Router {
  routes: { [key: string]: route };
  currentRoute?: route;
  private loaded: boolean = false;

  // called once when the app is launched to load the inital page. Any calls made afterwards does nothing.
  load() {
    if (!this.loaded) {
      this.currentRoute?.page.render();
      this.loaded = true;
    }
  }

  constructor(routes: { [key: string]: route }, startingPage: string = "/") {
    this.routes = routes;

    this.currentRoute = routes[startingPage];
  }

  go(path: string) {
    // check if the route is in the routes (TODO route parameter support)
    // also TODO error handling
    // also TODO metadata
    if (Object.keys(this.routes).includes(path)) {
      this.routes[this.currentRoute!.path].page._unload();
      this.routes[path].page.render();
    }
  }
}

export const router = new Router({
  "/": {
    page: new MainPage("Main Page"),
    path: "/",
  },
  "/second": {
    page: new SecondPage("Second Page"),
    path: "/second",
  },
});
