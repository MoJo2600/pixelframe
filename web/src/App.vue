<template>
  <v-app>
    <v-app-bar app flat>
      <v-app-bar-nav-icon
        class="hidden-lg-and-up"
        @click="drawerVisible = !drawerVisible"
      ></v-app-bar-nav-icon>
    </v-app-bar>

    <navigation v-model="drawerVisible" />

    <v-main :style="{ background }">
      <router-view></router-view>
    </v-main>
  </v-app>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import Navigation from "@/components/Navigation.vue";
import { VuetifyThemeItem } from "vuetify/types/services/theme";

@Component({
  components: {
    Navigation
  }
})
export default class App extends Vue {
  private drawerVisible = true;

  get background(): VuetifyThemeItem | undefined {
    const currentTheme = this.$vuetify.theme.dark ? "dark" : "light";
    return this.$vuetify.theme.themes[currentTheme].background;
  }
}
</script>

<style lang="scss">
.skeleton-child-full-width {
  & > div {
    width: 100%;
  }
}
</style>
