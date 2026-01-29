<template>
  <v-container>
    <v-row>
      <v-col cols="12">
        <h4 class="text-h4">
          MQTT configuration
          <spinner-text text="loading" v-if="loading" />
        </h4>
      </v-col>
    </v-row>

    <data-loader-error v-if="error" text="Failed to load MQTT configuration" />

    <configuration-section :error="error">
      <v-form v-model="formValid" @submit.prevent="updateMqttConfiguration">
        <configuration-input-wrapper
          title="Host"
          skeletonType="input"
          :error="error"
          :loading="loading"
        >
          <v-text-field
            v-if="!loading && !error && mqttConfiguration"
            v-model="mqttConfiguration.host"
            outlined
            dense
            :rules="[required]"
            type="text"
            hide-details
          ></v-text-field>
        </configuration-input-wrapper>

        <configuration-input-wrapper
          title="Port"
          skeletonType="input"
          :error="error"
          :loading="loading"
        >
          <v-text-field
            v-if="!loading && !error && mqttConfiguration"
            v-model="mqttConfiguration.port"
            outlined
            dense
            :rules="[required, validPort]"
            type="number"
            hide-details
          ></v-text-field>
        </configuration-input-wrapper>

        <configuration-input-wrapper
          title="User"
          skeletonType="input"
          :error="error"
          :loading="loading"
        >
          <v-text-field
            v-if="!loading && !error && mqttConfiguration"
            v-model="mqttConfiguration.user"
            outlined
            dense
            :rules="[required]"
            type="text"
            hide-details
          ></v-text-field>
        </configuration-input-wrapper>

        <configuration-input-wrapper
          title="Password"
          skeletonType="input"
          :error="error"
          :loading="loading"
        >
          <v-text-field
            v-if="!loading && !error && mqttConfiguration"
            v-model="mqttConfiguration.password"
            outlined
            dense
            :rules="[required]"
            :append-icon="showPassword ? 'mdi-eye' : 'mdi-eye-off'"
            @click:append="showPassword = !showPassword"
            :type="showPassword ? 'text' : 'password'"
            hide-details
          ></v-text-field>
        </configuration-input-wrapper>

        <configuration-input-wrapper
          title="Connection topic"
          skeletonType="input"
          :error="error"
          :loading="loading"
        >
          <v-text-field
            v-if="!loading && !error && mqttConfiguration"
            v-model="mqttConfiguration.baseTopic"
            outlined
            dense
            :rules="[required]"
            type="text"
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
import { ref, onMounted } from 'vue'
import SpinnerText from '@/components/SpinnerText.vue'
import DataLoaderError from '@/components/DataLoaderError.vue'
import ConfigurationSection from '@/components/ConfigurationSection.vue'
import ConfigurationInputWrapper from '@/components/ConfigurationInputWrapper.vue'
import ConfigurationFormButton from '@/components/ConfigurationFormButton.vue'
import { useDataHandler, WriteAction } from '@/mixins'
import { UpdateMqttConfiguration } from '@/models/configuration'
import { Service, ConfigurationService } from '@/services'
import { required, validPort } from '@/validation'

const { loading, error, writing, wrapDataRead, wrapDataWrite } =
  useDataHandler()
const configService = Service.get(ConfigurationService)

const mqttConfiguration = ref<UpdateMqttConfiguration | null>(null)
const showPassword = ref(false)
const formValid = ref(false)

async function updateMqttConfiguration(): Promise<void> {
  await wrapDataWrite(
    async () => {
      if (!mqttConfiguration.value) {
        return
      }

      await configService.updateMqttConfiguration(mqttConfiguration.value)
    },
    WriteAction.Update,
    'MQTT configuration'
  )
}

onMounted(async () => {
  await wrapDataRead(async () => {
    mqttConfiguration.value = {
      ...(await configService.getMqttConfiguration()),
      password: '',
    }
  })
})
</script>
