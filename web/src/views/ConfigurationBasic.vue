<template>
  <v-container>
    <v-row>
      <v-col cols="12">
        <h4 class="text-h4">
          Basic configuration
          <spinner-text text="loading" v-if="loading" />
        </h4>
      </v-col>
    </v-row>

    <data-loader-error v-if="error" text="Failed to load basic configuration" />

    <configuration-section title="Lighting" :error="error">
      <configuration-input-wrapper
        title="Brightness"
        skeletonType="slider"
        :error="error"
        :loading="loading"
      >
        <v-slider
          v-if="!loading && !error && basicConfiguration"
          min="0"
          max="255"
          v-model="basicConfiguration.brightness"
          hide-details
          @end="updateBrightness"
        ></v-slider>
      </configuration-input-wrapper>

      <v-row style="margin-top: -24px">
        <v-col
          :cols="display.xs.value ? 12 : 8"
          :offset="display.xs.value ? 0 : 4"
        >
          <v-row>
            <v-col cols="6">
              <v-skeleton-loader
                v-if="loading || error"
                :boilerplate="error"
                class="mx-auto mt-1 mb-3"
                max-width="200"
                type="text"
              ></v-skeleton-loader>
              <p
                v-else-if="basicConfiguration"
                class="text-caption text-center"
              >
                Absolute value: {{ basicConfiguration.brightness }}
              </p>
            </v-col>
            <v-col cols="6">
              <v-skeleton-loader
                v-if="loading || error"
                :boilerplate="error"
                class="mx-auto mt-1 mb-3"
                max-width="200"
                type="text"
              ></v-skeleton-loader>
              <p
                v-else-if="basicConfiguration"
                class="text-caption text-center"
              >
                Percent value:
                {{ ((basicConfiguration.brightness / 255) * 100).toFixed(0) }}%
              </p>
            </v-col>
          </v-row>
        </v-col>
      </v-row>
    </configuration-section>

    <configuration-section title="Clock" :error="error">
      <configuration-input-wrapper
        title="Timezone"
        skeletonType="input"
        :error="error"
        :loading="loading"
      >
        <v-autocomplete
          v-if="!loading && !error && basicConfiguration"
          v-model="basicConfiguration.timezone"
          :items="timezoneItems"
          outlined
          dense
          hide-details
          @change="updateTimezone"
        ></v-autocomplete>
      </configuration-input-wrapper>
    </configuration-section>

    <configuration-section title="Frames" :error="error">
      <configuration-input-wrapper
        title="Default mode"
        skeletonType="input"
        :error="error"
        :loading="loading"
      >
        <v-autocomplete
          v-if="!loading && !error && basicConfiguration"
          v-model="basicConfiguration.defaultMode"
          :items="basicConfiguration.availableDefaultModes"
          outlined
          dense
          hide-details
          @change="updateDefaultMode"
        ></v-autocomplete>
      </configuration-input-wrapper>
    </configuration-section>
  </v-container>
</template>

<script setup lang="ts">
import { ref, onMounted } from 'vue'
import { useDisplay } from 'vuetify'
import SpinnerText from '@/components/SpinnerText.vue'
import DataLoaderError from '@/components/DataLoaderError.vue'
import ConfigurationSection from '@/components/ConfigurationSection.vue'
import ConfigurationInputWrapper from '@/components/ConfigurationInputWrapper.vue'
import { useDataHandler, WriteAction } from '@/mixins'
import { BasicConfiguration } from '@/models/configuration'
import { Service, ConfigurationService } from '@/services'
import timezones from '@/assets/timezones.json'

const { loading, error, wrapDataRead, wrapDataWrite } = useDataHandler()
const configService = Service.get(ConfigurationService)
const display = useDisplay()

const basicConfiguration = ref<BasicConfiguration | null>(null)
const timezoneItems = timezones.map(t => ({
  title: t,
  value: t,
}))

async function updateBrightness(): Promise<void> {
  await wrapDataWrite(
    async () => {
      if (!basicConfiguration.value) {
        return
      }

      await configService.updateBasicConfiguratin({
        brightness: basicConfiguration.value.brightness,
      })
    },
    WriteAction.Update,
    'brightness'
  )
}

async function updateTimezone(): Promise<void> {
  await wrapDataWrite(
    async () => {
      if (!basicConfiguration.value) {
        return
      }

      await configService.updateBasicConfiguratin({
        timezone: basicConfiguration.value.timezone,
      })
    },
    WriteAction.Update,
    'timezone'
  )
}

async function updateDefaultMode(): Promise<void> {
  await wrapDataWrite(
    async () => {
      if (!basicConfiguration.value) {
        return
      }

      await configService.updateBasicConfiguratin({
        defaultMode: basicConfiguration.value.defaultMode,
      })
    },
    WriteAction.Update,
    'default mode'
  )
}

onMounted(async () => {
  await wrapDataRead(async () => {
    basicConfiguration.value = await configService.getBasicConfiguration()
  })
})
</script>
