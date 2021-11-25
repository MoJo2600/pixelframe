<template>
  <v-navigation-drawer v-model="drawerVisible" app hide-overlay>
    <v-list nav>
      <template
        v-for="item in ($router.options.routes || []).filter(
          r => r.path !== '/'
        )"
      >
        <v-list-item
          v-if="!item.children"
          :key="item.name + '-item'"
          :to="item.path"
        >
          <v-list-item-icon>
            <v-icon>{{ item.meta.icon }}</v-icon>
          </v-list-item-icon>

          <v-list-item-content>
            <v-list-item-title>{{ item.name }}</v-list-item-title>
          </v-list-item-content>
        </v-list-item>

        <v-list-group
          v-else
          :group="item.path"
          :key="item.name + '-group'"
          :prepend-icon="item.meta.icon"
        >
          <template #activator>
            <v-list-item-title>{{ item.name }}</v-list-item-title>
          </template>

          <v-list-item
            v-for="nestedItem in item.children"
            :key="nestedItem.name"
            :to="`${item.path}/${nestedItem.path}`"
            dense
          >
            <v-list-item-content>
              <v-list-item-title class="ml-2">{{
                nestedItem.name
              }}</v-list-item-title>
            </v-list-item-content>
          </v-list-item>
        </v-list-group>
      </template>

      <v-list-item href="/update">
        <v-list-item-icon>
          <v-icon>mdi-memory</v-icon>
        </v-list-item-icon>

        <v-list-item-content>
          <v-list-item-title>Update firmware</v-list-item-title>
        </v-list-item-content>
      </v-list-item>

      <v-list-item @click="ThemeModule.switchTheme()">
        <v-list-item-icon>
          <v-icon>mdi-theme-light-dark</v-icon>
        </v-list-item-icon>

        <v-list-item-content>
          <v-list-item-title>Theme</v-list-item-title>
        </v-list-item-content>
      </v-list-item>

      <v-list-item @click="shutdown">
        <v-list-item-icon>
          <v-icon>mdi-power</v-icon>
        </v-list-item-icon>

        <v-list-item-content>
          <v-list-item-title>Off</v-list-item-title>
        </v-list-item-content>
      </v-list-item>
    </v-list>
  </v-navigation-drawer>
</template>

<script lang="ts">
import { FramesService, Service } from "@/services";
import { Vue, Component, VModel, Watch } from "vue-property-decorator";
import ThemeModule from "@/store/modules/theme";

@Component
export default class Navigation extends Vue {
  ThemeModule = ThemeModule;

  private readonly framesService = Service.get(FramesService);

  @VModel()
  public readonly drawerVisible!: boolean;

  public async shutdown(): Promise<void> {
    await this.framesService.showFrame("off");
  }

  get getTheme() {
    return ThemeModule.darkThemeEnabled
  }

  @Watch("getTheme")
  onThemeChanged() {
      this.$vuetify.theme.dark = this.getTheme;
  }

}
</script>

<style lang="scss" scoped>
.v-list-item--active {
  color: var(--v-primary-base) !important;
}

.v-list-item--active::before {
  background: var(--v-primary-base);
}
</style>
