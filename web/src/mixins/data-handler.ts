import Vue from "vue";
import Component from "vue-class-component";
import NotificationModule, {
  NotificationType
} from "@/store/modules/notification";

export enum WriteAction {
  Create = "create",
  Update = "update",
  Delete = "delete"
}

@Component
export class DataHandlerMixin extends Vue {
  // data loading
  protected loading = true;
  protected error = false;
  // data sending / updating / deleting
  protected writing: string | null = null;

  protected async wrapDataRead(fn: () => Promise<void>): Promise<void> {
    try {
      await fn();
    } catch (error) {
      this.error = true;

      NotificationModule.notify({
        type: NotificationType.Error,
        content: "Failed to load data",
        details: error.message
      });
    } finally {
      this.loading = false;
    }
  }

  protected async wrapDataWrite(
    fn: () => Promise<void>,
    action: WriteAction,
    item: string
  ): Promise<void> {
    try {
      this.writing = item;

      await fn();

      NotificationModule.notify({
        type: NotificationType.Success,
        content: `Successfully ${action}d ${item}`
      });
    } catch (error) {
      NotificationModule.notify({
        type: NotificationType.Error,
        content: `Failed to ${action} ${item}`,
        details: error.message
      });
    } finally {
      this.writing = null;
    }
  }
}
