import { v4 as uuidv4 } from "uuid";
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
  name: "renderer",
  store
})
class RendererModule extends VuexModule {
  private key: string = uuidv4();

  get renderKey(): string {
    return this.key;
  }

  @Mutation
  private assignNewKey(key: string): void {
    this.key = key;
  }

  @Action({ commit: "assignNewKey" })
  public forceReRender(): string {
    return uuidv4();
  }
}

export default getModule(RendererModule);
