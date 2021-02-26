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
            v-if="!loading && !error"
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
            v-if="!loading && !error"
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
            v-if="!loading && !error"
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
            v-if="!loading && !error"
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
            v-if="!loading && !error"
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

<script lang="ts">
import Component from "vue-class-component";
import { Mixins } from "vue-property-decorator";
import SpinnerText from "@/components/SpinnerText.vue";
import DataLoaderError from "@/components/DataLoaderError.vue";
import ConfigurationSection from "@/components/ConfigurationSection.vue";
import ConfigurationInputWrapper from "@/components/ConfigurationInputWrapper.vue";
import ConfigurationFormButton from "@/components/ConfigurationFormButton.vue";
import { DataHandlerMixin, WriteAction } from "@/mixins";
import { UpdateMqttConfiguration } from "@/models/configuration";
import { Service, ConfigurationService } from "@/services";
import { required, validPort } from "@/validation";

@Component({
  components: {
    SpinnerText,
    DataLoaderError,
    ConfigurationSection,
    ConfigurationInputWrapper,
    ConfigurationFormButton
  }
})
export default class MqttConfigurationView extends Mixins(DataHandlerMixin) {
  required = required;
  validPort = validPort;

  private readonly configService = Service.get(ConfigurationService);
  private mqttConfiguration: UpdateMqttConfiguration | null = null;
  private showPassword = false;
  private formValid = false;

  private async updateMqttConfiguration(): Promise<void> {
    await this.wrapDataWrite(
      async () => {
        if (!this.mqttConfiguration) {
          return;
        }

        await this.configService.updateMqttConfiguration(
          this.mqttConfiguration
        );
      },
      WriteAction.Update,
      "MQTT configuration"
    );
  }

  private async created(): Promise<void> {
    await this.wrapDataRead(async () => {
      this.mqttConfiguration = {
        ...(await this.configService.getMqttConfiguration()),
        password: ""
      };
    });
  }
}
</script>
