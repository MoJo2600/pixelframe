import {
  Action,
  getModule,
  Module,
  Mutation,
  VuexModule
} from "vuex-module-decorators";
import store from "@/store";

@Module({
  dynamic: true,
  namespaced: true,
  name: "theme",
  store
})
class ThemeModule extends VuexModule {
  private darkTheme = false;

  get darkThemeEnabled(): boolean {
    return this.darkTheme;
  }

  @Mutation
  private setDarkTheme(darkTheme: boolean): void {
    this.darkTheme = darkTheme;
    // * TODO localStorage.put
  }

  @Action({ commit: "setDarkTheme" })
  public switchTheme(): boolean {
    return !this.darkTheme;
  }
}

export default getModule(ThemeModule);
