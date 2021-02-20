import Vue from "vue";
import VueRouter, { RouteConfig } from "vue-router";
import NestedViewWrapper from "@/components/NestedViewWrapper.vue";
import ConfigurationBasic from "@/views/ConfigurationBasic.vue";
import ConfigurationWifi from "@/views/ConfigurationWifi.vue";
import ConfigurationMqtt from "@/views/ConfigurationMqtt.vue";

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
        component: ConfigurationBasic
      },
      {
        path: "wifi",
        name: "WiFi",
        component: ConfigurationWifi
      },
      {
        path: "mqtt",
        name: "MQTT",
        component: ConfigurationMqtt
      }
    ]
  }
];

const router = new VueRouter({
  mode: "history",
  base: process.env.BASE_URL,
  routes
});

export default router;
