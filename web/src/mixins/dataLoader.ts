import Vue from "vue";
import Component from "vue-class-component";

@Component
export class DataLoaderMixin extends Vue {
  protected loading = true;
  protected error = false;

  protected async wrapDataLoading(fn: () => Promise<void>): Promise<void> {
    try {
      await fn();
    } catch (error) {
      // console.log(error);
      this.error = true;
    } finally {
      this.loading = false;
    }
  }
}
