import Vue from "vue";
import VueRouter, { RouteConfig } from "vue-router";
import NestedViewWrapper from "@/components/NestedViewWrapper.vue";
import ConfigurationBasicView from "@/views/ConfigurationBasic.vue";
import ConfigurationWifiView from "@/views/ConfigurationWifi.vue";
import ConfigurationMqttView from "@/views/ConfigurationMqtt.vue";
import ImagesView from "@/views/Images.vue";

Vue.use(VueRouter);

const routes: Array<RouteConfig> = [
  {
    path: "/configuration",
    name: "Configuration",
    component: NestedViewWrapper,
    meta: {
      icon: "mdi-cog"
    },
    children: [
      {
        path: "basic",
        name: "Basic",
        component: ConfigurationBasicView
      },
      {
        path: "wifi",
        name: "WiFi",
        component: ConfigurationWifiView
      },
      {
        path: "mqtt",
        name: "MQTT",
        component: ConfigurationMqttView
      }
    ]
  },
  {
    path: "/images",
    name: "Images",
    component: ImagesView,
    meta: {
      icon: "mdi-image"
    }
  }
];

const router = new VueRouter({
  mode: "history",
  base: process.env.BASE_URL,
  routes
});

export default router;
