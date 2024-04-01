import { Page } from "../renderer/page";

export type route = {
  page: Page;
  path: string;
  metadata?: { [key: string]: any };
};
