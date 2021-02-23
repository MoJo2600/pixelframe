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

    <v-row>
      <v-col cols="12">
        <v-card :disabled="error" elevation="0" outlined>
          <v-form v-model="formValid" @submit.prevent="updateMqttConfiguration">
            <v-card-text>
              <v-row>
                <v-col :cols="$vuetify.breakpoint.xs ? 12 : 4">
                  <p class="text-body-1">Host</p>
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
                    v-model="mqttConfiguration.host"
                    outlined
                    dense
                    :rules="[required]"
                    type="text"
                    hide-details
                  ></v-text-field>
                </v-col>
              </v-row>

              <v-row>
                <v-col :cols="$vuetify.breakpoint.xs ? 12 : 4">
                  <p class="text-body-1">Port</p>
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
                    v-model="mqttConfiguration.port"
                    outlined
                    dense
                    :rules="[required, validPort]"
                    type="number"
                    hide-details
                  ></v-text-field>
                </v-col>
              </v-row>

              <v-row>
                <v-col :cols="$vuetify.breakpoint.xs ? 12 : 4">
                  <p class="text-body-1">User</p>
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
                    v-model="mqttConfiguration.user"
                    outlined
                    dense
                    :rules="[required]"
                    type="text"
                    hide-details
                  ></v-text-field>
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
                    v-model="mqttConfiguration.password"
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
                <v-col :cols="$vuetify.breakpoint.xs ? 12 : 4">
                  <p class="text-body-1">Base topic</p>
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
                    v-model="mqttConfiguration.baseTopic"
                    outlined
                    dense
                    :rules="[required]"
                    type="text"
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
import Component from "vue-class-component";
import { Mixins } from "vue-property-decorator";
import SpinnerText from "@/components/SpinnerText.vue";
import DataLoaderError from "@/components/DataLoaderError.vue";
import { DataHandlerMixin, WriteAction } from "@/mixins";
import { UpdateMqttConfiguration } from "@/models/configuration";
import { Service, ConfigurationService } from "@/services";
import { required, validPort } from "@/validation";

@Component({
  components: {
    SpinnerText,
    DataLoaderError
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
