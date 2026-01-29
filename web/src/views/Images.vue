<template>
  <v-container>
    <v-row>
      <v-col cols="12">
        <h4 class="text-h4">
          Images
          <spinner-text text="loading" v-if="loading" />
        </h4>
      </v-col>
    </v-row>

    <data-loader-error v-if="error" text="Failed to load images" />

    <configuration-section title="Upload image" :error="error">
      <v-form v-model="formValid" @submit.prevent="uploadImage">
        <v-row>
          <v-col cols="12">
            <v-file-input
              label="File"
              v-model="file"
              outlined
              :rules="[required]"
              accept="image/*"
              hide-details
            ></v-file-input>
          </v-col>
        </v-row>

        <configuration-form-button
          :error="error"
          :loading="loading"
          :formValid="formValid"
          :writing="writing"
          title="Upload"
        />
      </v-form>
    </configuration-section>

    <configuration-section :error="error">
      <v-row>
        <v-col :cols="imageSize" v-for="image in images" :key="image.name">
          <v-hover>
            <template v-slot:default="{ isHovering }">
              <v-container class="ma-0 pa-0" style="position: relative">
                <v-img
                  class="pixelated"
                  :class="{ 'image-hover': isHovering }"
                  :src="imageHost + '/' + image.name"
                ></v-img>

                <v-fade-transition>
                  <v-overlay v-if="isHovering" absolute>
                    <v-btn
                      icon
                      large
                      color="white"
                      @click="playImage('/' + image.name)"
                    >
                      <v-icon>mdi-play</v-icon>
                    </v-btn>

                    <v-btn
                      icon
                      large
                      color="red"
                      @click="deleteImage(image.name)"
                    >
                      <v-icon>mdi-delete</v-icon>
                    </v-btn>
                  </v-overlay>
                </v-fade-transition>
              </v-container>
            </template>
          </v-hover>
        </v-col>
      </v-row>
    </configuration-section>
  </v-container>
</template>

<script setup lang="ts">
import { ref, computed, onMounted } from 'vue'
import { useDisplay } from 'vuetify'
import SpinnerText from '@/components/SpinnerText.vue'
import DataLoaderError from '@/components/DataLoaderError.vue'
import ConfigurationSection from '@/components/ConfigurationSection.vue'
import ConfigurationFormButton from '@/components/ConfigurationFormButton.vue'
import { useDataHandler, WriteAction } from '@/mixins'
import { Image } from '@/models/images'
import { Service, ImagesService } from '@/services'
import { required } from '@/validation'

const { loading, error, writing, wrapDataRead, wrapDataWrite } =
  useDataHandler()
const imgService = Service.get(ImagesService)
const display = useDisplay()

const imageHost =
  (import.meta.env.VITE_API_BASE_URL as string) || window.location.origin
const images = ref<Image[]>([])
const file = ref<File[] | null>(null)
const formValid = ref(false)

const imageSize = computed(() => {
  if (display.xs.value) return 6
  if (display.sm.value) return 4
  if (display.md.value) return 4
  if (display.lg.value) return 2
  return 1
})

async function playImage(name: string): Promise<void> {
  await wrapDataWrite(
    async () => {
      await imgService.playImage(name)
    },
    WriteAction.Command,
    `play image ${name}`
  )
}

async function deleteImage(name: string): Promise<void> {
  await wrapDataWrite(
    async () => {
      await imgService.deleteImage(name)
    },
    WriteAction.Delete,
    `image ${name}`
  )
}

async function uploadImage(): Promise<void> {
  console.log(file.value)
}

onMounted(async () => {
  await wrapDataRead(async () => {
    images.value = await imgService.listImages()
  })
})
</script>

<style lang="scss" scoped>
.pixelated {
  background: #000000;
}

:deep(.v-image__image) {
  image-rendering: pixelated;
}

.image-hover {
  transition:
    opacity 300ms,
    filter 300ms;
  filter: grayscale(100%);
  opacity: 0.4;
}
</style>
