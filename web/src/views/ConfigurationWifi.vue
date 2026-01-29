<template>
  <v-container>
    <v-row>
      <v-col cols="12">
        <h4 class="text-h4">
          WiFi configuration
          <spinner-text text="loading" v-if="loading" />
        </h4>
      </v-col>
    </v-row>

    <data-loader-error v-if="error" text="Failed to load WiFi configuration" />

    <configuration-section :error="error">
      <v-form v-model="formValid" @submit.prevent="updateWifiConfiguration">
        <configuration-input-wrapper
          title="SSID"
          skeletonType="input"
          :error="error"
          :loading="loading"
        >
          <v-autocomplete
            v-if="!loading && !error && wifiConfiguration"
            v-model="wifiConfiguration.ssid"
            :items="wifiItems"
            outlined
            dense
            hide-details
            :rules="[required]"
          ></v-autocomplete>
        </configuration-input-wrapper>

        <configuration-input-wrapper
          title="Password"
          skeletonType="input"
          :error="error"
          :loading="loading"
        >
          <v-text-field
            v-if="!loading && !error && wifiConfiguration"
            v-model="wifiConfiguration.password"
            outlined
            dense
            :rules="[required]"
            :append-icon="showPassword ? 'mdi-eye' : 'mdi-eye-off'"
            @click:append="showPassword = !showPassword"
            :type="showPassword ? 'text' : 'password'"
            hide-details
          ></v-text-field>
        </configuration-input-wrapper>

        <configuration-form-button
          :error="error"
          :loading="loading"
          :formValid="formValid"
          :writing="writing"
          title="Save"
        />
      </v-form>
    </configuration-section>
  </v-container>
</template>

<script setup lang="ts">
import { ref, computed, onMounted } from 'vue'
import { orderBy } from 'lodash'
import SpinnerText from '@/components/SpinnerText.vue'
import DataLoaderError from '@/components/DataLoaderError.vue'
import ConfigurationSection from '@/components/ConfigurationSection.vue'
import ConfigurationInputWrapper from '@/components/ConfigurationInputWrapper.vue'
import ConfigurationFormButton from '@/components/ConfigurationFormButton.vue'
import { useDataHandler, WriteAction } from '@/mixins'
import { UpdateWifiConfiguration } from '@/models/configuration'
import { Service, ConfigurationService, EnvironmentService } from '@/services'
import { Wifi } from '@/models/environment'
import { required } from '@/validation'

const { loading, error, writing, wrapDataRead, wrapDataWrite } =
  useDataHandler()
const configService = Service.get(ConfigurationService)
const environmentService = Service.get(EnvironmentService)

const wifiConfiguration = ref<UpdateWifiConfiguration | null>(null)
const availableWifis = ref<Wifi[]>([])
const showPassword = ref(false)
const formValid = ref(false)

const wifiItems = computed(() => {
  return orderBy(availableWifis.value, ['signalStrength'], 'desc').map(w => ({
    title: w.ssid,
    value: w.ssid,
  }))
})

async function updateWifiConfiguration(): Promise<void> {
  await wrapDataWrite(
    async () => {
      if (!wifiConfiguration.value) {
        return
      }

      await configService.updateWifiConfiguration(wifiConfiguration.value)
    },
    WriteAction.Update,
    'WiFi configuration'
  )
}

onMounted(async () => {
  await wrapDataRead(async () => {
    wifiConfiguration.value = {
      ...(await configService.getWifiConfiguration()),
      password: '',
    }
    availableWifis.value = await environmentService.getAvailableWifis()
  })
})
</script>
