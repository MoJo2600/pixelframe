<template>
  <v-expand-transition>
    <v-row>
      <v-col cols="6" offset="3">
        <v-card elevation="0" style="background: none" outlined>
          <v-card-text>
            <h6 class="text-h6 text-center">
              {{ text }}
            </h6>
            <v-flex text-center align-center>
              <v-btn color="accent" plain @click="retry">
                <v-icon left color="accent"> mdi-refresh </v-icon>Retry
              </v-btn>
            </v-flex>
          </v-card-text>
        </v-card>
      </v-col>
    </v-row>
  </v-expand-transition>
</template>

<script setup lang="ts">
import { useRendererStore } from '@/store/modules/renderer'

defineProps<{
  text: string
}>()

const rendererStore = useRendererStore()

function retry(): void {
  rendererStore.forceReRender()
}
</script>
