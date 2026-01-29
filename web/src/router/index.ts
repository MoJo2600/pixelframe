import { createRouter, createWebHashHistory, RouteRecordRaw } from 'vue-router'
import NestedViewWrapper from '@/components/NestedViewWrapper.vue'
import FramesView from '@/views/Frames.vue'
import VisualsView from '@/views/Visuals.vue'
import ConfigurationBasicView from '@/views/ConfigurationBasic.vue'
import ConfigurationWifiView from '@/views/ConfigurationWifi.vue'
import ConfigurationMqttView from '@/views/ConfigurationMqtt.vue'
import ImagesView from '@/views/Images.vue'

const routes: Array<RouteRecordRaw> = [
  {
    path: '/',
    redirect: {
      path: '/frames'
    }
  },
  {
    path: '/frames',
    name: 'Frames',
    component: FramesView,
    meta: {
      icon: 'mdi-view-grid'
    }
  },
  {
    path: '/visuals',
    name: 'Visuals',
    component: VisualsView,
    meta: {
      icon: 'mdi-blur'
    }
  },
  {
    path: '/images',
    name: 'Images',
    component: ImagesView,
    meta: {
      icon: 'mdi-image'
    }
  },
  {
    path: '/configuration',
    name: 'Configuration',
    component: NestedViewWrapper,
    meta: {
      icon: 'mdi-cog'
    },
    children: [
      {
        path: 'basic',
        name: 'Basic',
        component: ConfigurationBasicView
      },
      {
        path: 'wifi',
        name: 'WiFi',
        component: ConfigurationWifiView
      },
      {
        path: 'mqtt',
        name: 'MQTT',
        component: ConfigurationMqttView
      }
    ]
  }
]

const router = createRouter({
  history: createWebHashHistory(),
  routes
})

export default router
