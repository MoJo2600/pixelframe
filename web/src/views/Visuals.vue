<template>
  <v-container>
    <v-row>
      <v-col cols="12">
        <h4 class="text-h4">
          Visuals
          <spinner-text text="loading" v-if="loading" />
          <spinner-text text="sending" v-if="writing" />
        </h4>
      </v-col>
    </v-row>

    <v-row>
      <v-col :cols="cardSize" v-for="visual in visuals" :key="visual.name">
        <v-card elevation="0" outlined @click="showVisual(visual)">
          <v-img
            :src="`https://picsum.photos/seed/${visual.name}/600/300`"
            height="200px"
          ></v-img>

          <v-card-title>
            {{ visual.title }}
          </v-card-title>
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
import { Service, VisualsService } from '@/services'

interface Visual {
  title: string
  name: string
}

const { loading, writing, wrapDataRead, wrapDataWrite } = useDataHandler()
const visualsService = Service.get(VisualsService)
const display = useDisplay()

// TODO: Load this from the backend
const visuals: Visual[] = [
  {
    title: 'Bpm',
    name: 'bpm',
  },
  {
    title: 'Twinkle',
    name: 'twinkle',
  },
  {
    title: 'Pacifica',
    name: 'pacifica',
  },
  {
    title: 'Rainbow Beat',
    name: 'rainbowbeat',
  },
  {
    title: 'Confetti',
    name: 'confetti',
  },
  {
    title: 'Noise',
    name: 'noise',
  },
  {
    title: 'Random',
    name: 'random',
  },
]

const cardSize = computed(() => {
  if (display.xs.value) return 12
  if (display.sm.value) return 6
  return 3
})

async function showVisual(visual: Visual): Promise<void> {
  await wrapDataWrite(
    async () => {
      await visualsService.showVisual(visual.name)
    },
    WriteAction.Command,
    `show visual ${visual.name}`
  )
}

onMounted(async () => {
  await wrapDataRead(async () => {
    // nothing to load, required for setting loading to false
  })
})
</script>
