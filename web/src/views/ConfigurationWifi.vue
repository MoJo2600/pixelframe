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
            v-if="!loading && !error"
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
            v-if="!loading && !error"
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

<script lang="ts">
import { orderBy } from "lodash";
import Component from "vue-class-component";
import { Mixins } from "vue-property-decorator";
import SpinnerText from "@/components/SpinnerText.vue";
import DataLoaderError from "@/components/DataLoaderError.vue";
import ConfigurationSection from "@/components/ConfigurationSection.vue";
import ConfigurationInputWrapper from "@/components/ConfigurationInputWrapper.vue";
import ConfigurationFormButton from "@/components/ConfigurationFormButton.vue";
import { DataHandlerMixin, WriteAction } from "@/mixins";
import { UpdateWifiConfiguration } from "@/models/configuration";
import { Service, ConfigurationService, EnvironmentService } from "@/services";
import { Wifi } from "@/models/environment";
import { required } from "@/validation";

@Component({
  components: {
    SpinnerText,
    DataLoaderError,
    ConfigurationSection,
    ConfigurationInputWrapper,
    ConfigurationFormButton
  }
})
export default class WifiConfigurationView extends Mixins(DataHandlerMixin) {
  required = required;

  private readonly configService = Service.get(ConfigurationService);
  private readonly environmentService = Service.get(EnvironmentService);
  private wifiConfiguration: UpdateWifiConfiguration | null = null;
  private availableWifis: Wifi[] = [];
  private showPassword = false;
  private formValid = false;

  private get wifiItems() {
    return orderBy(this.availableWifis, ["signalStrength"], "desc").map(w => {
      return {
        text: w.ssid,
        value: w.ssid
      };
    });
  }

  private async updateWifiConfiguration(): Promise<void> {
    await this.wrapDataWrite(
      async () => {
        if (!this.wifiConfiguration) {
          return;
        }

        await this.configService.updateWifiConfiguration(
          this.wifiConfiguration
        );
      },
      WriteAction.Update,
      "WiFi configuration"
    );
  }

  private async created(): Promise<void> {
    await this.wrapDataRead(async () => {
      this.wifiConfiguration = {
        ...(await this.configService.getWifiConfiguration()),
        password: ""
      };
      this.availableWifis = await this.environmentService.getAvailableWifis();
    });
  }
}
</script>
