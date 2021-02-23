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

    <v-row>
      <v-col cols="12">
        <v-card :disabled="error" elevation="0" outlined>
          <v-form v-model="formValid" @submit.prevent="updateWifiConfiguration">
            <v-card-text>
              <v-row>
                <v-col :cols="$vuetify.breakpoint.xs ? 12 : 4">
                  <p class="text-body-1">SSID</p>
                </v-col>
                <v-col :cols="$vuetify.breakpoint.xs ? 12 : 8">
                  <v-skeleton-loader
                    v-if="loading || error"
                    :boilerplate="error"
                    class="skeleton-child-full-width"
                    type="button"
                  ></v-skeleton-loader>
                  <v-autocomplete
                    v-else
                    v-model="wifiConfiguration.ssid"
                    :items="wifiItems"
                    outlined
                    dense
                    hide-details
                    :rules="[required]"
                  ></v-autocomplete>
                </v-col>
              </v-row>

              <v-row>
                <v-col :cols="$vuetify.breakpoint.xs ? 12 : 4">
                  <p class="text-body-1">Password</p>
                </v-col>
                <v-col :cols="$vuetify.breakpoint.xs ? 12 : 8">
                  <v-skeleton-loader
                    v-if="loading || error"
                    :boilerplate="error"
                    class="skeleton-child-full-width"
                    type="button"
                  ></v-skeleton-loader>
                  <v-text-field
                    v-else
                    v-model="wifiConfiguration.password"
                    outlined
                    dense
                    :rules="[required]"
                    :append-icon="showPassword ? 'mdi-eye' : 'mdi-eye-off'"
                    @click:append="showPassword = !showPassword"
                    :type="showPassword ? 'text' : 'password'"
                    hide-details
                  ></v-text-field>
                </v-col>
              </v-row>

              <v-row>
                <v-col cols="12">
                  <v-btn
                    class="float-right"
                    color="primary"
                    type="submit"
                    :disabled="loading || error || !formValid"
                    :loading="!!writing"
                    depressed
                  >
                    Save
                  </v-btn>
                </v-col>
              </v-row>
            </v-card-text>
          </v-form>
        </v-card>
      </v-col>
    </v-row>
  </v-container>
</template>

<script lang="ts">
import { orderBy } from "lodash";
import Component from "vue-class-component";
import { Mixins } from "vue-property-decorator";
import SpinnerText from "@/components/SpinnerText.vue";
import DataLoaderError from "@/components/DataLoaderError.vue";
import { DataHandlerMixin, WriteAction } from "@/mixins";
import { UpdateWifiConfiguration } from "@/models/configuration";
import { Service, ConfigurationService, EnvironmentService } from "@/services";
import { Wifi } from "@/models/environment";
import { required } from "@/validation";

@Component({
  components: {
    SpinnerText,
    DataLoaderError
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
