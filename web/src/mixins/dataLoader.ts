import Vue from "vue";
import Component from "vue-class-component";
import NotificationModule, {
  NotificationType
} from "@/store/modules/notification";

@Component
export class DataLoaderMixin extends Vue {
  protected loading = true;
  protected error = false;

  protected async wrapDataLoading(fn: () => Promise<void>): Promise<void> {
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
}
