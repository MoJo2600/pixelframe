<template>
  <v-container>
    <v-row>
      <v-col cols="12">
        <h4 class="text-h4">
          Frames
          <spinner-text text="loading" v-if="loading" />
          <spinner-text text="sending" v-if="writing" />
        </h4>
      </v-col>
    </v-row>

    <v-row>
      <v-col :cols="cardSize" v-for="frame in frames" :key="frame.name">
        <v-card elevation="0" outlined @click="showFrame(frame)">
          <v-img
            :src="`https://picsum.photos/seed/${frame.apiPath}/600/300`"
            height="200px"
          ></v-img>

          <v-card-title>
            {{ frame.name }}
          </v-card-title>

          <v-card-subtitle>
            {{ frame.description }}
          </v-card-subtitle>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script setup lang="ts">
import { computed, onMounted } from 'vue'
import { useDisplay } from 'vuetify'
import SpinnerText from '@/components/SpinnerText.vue'
import { useDataHandler, WriteAction } from '@/mixins'
import { Service, FramesService } from '@/services'

interface Frame {
  name: string
  description: string
  apiPath: string
}

const { loading, writing, wrapDataRead, wrapDataWrite } = useDataHandler()
const framesService = Service.get(FramesService)
const display = useDisplay()

const frames: Frame[] = [
  {
    name: 'Pong Clock',
    description: 'A fancy pong clock',
    apiPath: 'clock',
  },
  {
    name: 'Gif',
    description: 'Plays random GIF animations.',
    apiPath: 'gif',
  },
  {
    name: 'Visuals',
    description: 'Plays random visuals.',
    apiPath: 'visuals',
  },
  {
    name: 'Off',
    description: 'Turn off the LEDs.',
    apiPath: 'off',
  },
]

const cardSize = computed(() => {
  if (display.xs.value) return 12
  if (display.sm.value) return 6
  return 3
})

async function showFrame(frame: Frame): Promise<void> {
  await wrapDataWrite(
    async () => {
      await framesService.showFrame(frame.apiPath)
    },
    WriteAction.Command,
    `show frame ${frame.name}`
  )
}

onMounted(async () => {
  await wrapDataRead(async () => {
    // nothing to load, required for setting loading to false
  })
})
</script>
