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
      <router-view :key="RendererModule.renderKey"></router-view>
    </v-main>

    <v-container id="notification-container">
      <notification
        v-for="notification in NotificationModule.allNotifications"
        :key="notification.id"
        :notification="notification"
      />
    </v-container>
  </v-app>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import Navigation from "@/components/Navigation.vue";
import Notification from "@/components/Notification.vue";
import { VuetifyThemeItem } from "vuetify/types/services/theme";
import NotificationModule from "@/store/modules/notification";
import RendererModule from "@/store/modules/renderer";
import ThemeModule from "@/store/modules/theme";

@Component({
  components: {
    Navigation,
    Notification
  }
})
export default class App extends Vue {
  NotificationModule = NotificationModule;
  RendererModule = RendererModule;

  private drawerVisible = true;

  get background(): VuetifyThemeItem | undefined {
    const currentTheme = this.$vuetify.theme.dark ? "dark" : "light";
    return this.$vuetify.theme.themes[currentTheme].background;
  }

  created() {
    this.$vuetify.theme.dark = ThemeModule.darkThemeEnabled;
  }
}
</script>

<style lang="scss">
.skeleton-child-full-width {
  & > div {
    width: 100%;
  }
}

#notification-container {
  width: auto;
  position: fixed;
  top: 0;
  right: 0;
  z-index: 99;
  max-width: 480px;
}
</style>
