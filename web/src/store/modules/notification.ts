import { v4 as uuidv4 } from "uuid";
import {
  Action,
  getModule,
  Module,
  Mutation,
  VuexModule
} from "vuex-module-decorators";
import store from "@/store";

export enum NotificationType {
  Success = "success",
  Information = "info",
  Warning = "warning",
  Error = "error"
}

export interface NotificationData {
  id: string;
  dateTime: Date;
  type: NotificationType;
  content: string;
  details?: string;
}

@Module({
  dynamic: true,
  namespaced: true,
  name: "notification",
  store
})
class NotificationModule extends VuexModule {
  private notifications: NotificationData[] = [];

  get allNotifications(): NotificationData[] {
    return this.notifications;
  }

  // TODO: provide getter for visible notifications only to clean up DOM

  @Mutation
  private addNotification(notification: NotificationData): void {
    this.notifications.push(notification);
  }

  @Action({ commit: "addNotification" })
  public notify(options: {
    type: NotificationType;
    content: string;
    details?: string;
  }): NotificationData {
    return {
      id: uuidv4(),
      dateTime: new Date(),
      ...options
    };
  }
}

export default getModule(NotificationModule);
