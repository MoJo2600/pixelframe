<template>
  <v-container>
    <v-stepper
      v-model="stepperCurrentStep"
      vertical
      class="elevation-0 transparent"
    >
      <v-stepper-step :complete="stepperCurrentStep > 1" step="1" editable>
        Information
      </v-stepper-step>

      <v-stepper-content step="1">
        <v-container>
          <v-row>
            <h1>Pixelframe</h1>
          </v-row>
          <v-row>
            <a
              href="https://github.com/MoJo2600/pixelframe"
              style="text-decoration: none"
            >
              <v-icon large>
                mdi-github
              </v-icon>
            </a>
          </v-row>
          <v-row class="pt-8">
            <v-img
              src="https://github.com/MoJo2600/pixelframe/raw/dev/docs/pixelframe.png"
            ></v-img>
          </v-row>
          <v-row>
            device information
          </v-row>
        </v-container>

        <v-container class="mt-4">
          <v-btn text disabled>
            Back
          </v-btn>
          <v-btn color="primary" @click="stepperCurrentStep++">
            Continue
          </v-btn>
        </v-container>
      </v-stepper-content>

      <v-stepper-step :complete="stepperCurrentStep > 2" step="2" editable>
        General
      </v-stepper-step>

      <v-stepper-content step="2">
        <v-form v-model="wifiFormValid">
          <p>Framename / Hostname</p>
          <v-text-field
            v-model="frameName"
            outlined
            hide-details
          ></v-text-field>

          <p>Brightness</p>
          <v-text-field
            v-model.number="brightness"
            outlined
            hide-details
          ></v-text-field>

          <p>Timezone</p>
          <v-text-field v-model="timezone" outlined hide-details></v-text-field>

          <p>Default mode</p>
          <v-text-field
            v-model="defaultMode"
            outlined
            hide-details
          ></v-text-field>
        </v-form>

        <v-container class="mt-4">
          <v-btn text @click="stepperCurrentStep--">
            Back
          </v-btn>
          <v-btn color="primary" @click="stepperCurrentStep++">
            Continue
          </v-btn>
        </v-container>
      </v-stepper-content>

      <v-stepper-step :complete="stepperCurrentStep > 3" step="3" editable>
        WiFi
      </v-stepper-step>

      <v-stepper-content step="3">
        <v-form v-model="wifiFormValid">
          <v-container>
            <v-radio-group v-model="selectedWifi">
              <v-card
                v-for="wifi of availableAndSortedWifis"
                :key="wifi.ssid"
                class="px-4"
                @click="selectedWifi = wifi"
              >
                <v-card-text>
                  <v-row class="wifi py-3">
                    <v-col cols="auto">
                      <v-icon
                        class="wifiIcon pr-2"
                        :class="{
                          selected:
                            selectedWifi && selectedWifi.ssid === wifi.ssid
                        }"
                        >mdi-wifi</v-icon
                      >
                    </v-col>
                    <v-col>
                      <v-row>
                        <p>{{ wifi.ssid }}</p>
                      </v-row>
                      <v-row class="mt-0"
                        ><small class="text--disabled"
                          >Signal strength: {{ wifi.signalStrength }}</small
                        ></v-row
                      >
                    </v-col>

                    <v-radio :value="wifi.ssid" class="d-none"></v-radio>
                  </v-row>
                </v-card-text>
              </v-card>
            </v-radio-group>

            <v-btn color="primary" @click="refreshWifiList">
              <v-icon left>
                mdi-refresh
              </v-icon>
              Refresh
            </v-btn>
          </v-container>

          <v-expand-transition>
            <v-container v-show="selectedWifi !== null" class="mt-4">
              <p>Password</p>
              <v-text-field
                v-model="wifiPassword"
                outlined
                :append-icon="showWifiPassword ? 'mdi-eye' : 'mdi-eye-off'"
                @click:append="showWifiPassword = !showWifiPassword"
                :type="showWifiPassword ? 'text' : 'password'"
                hide-details
              ></v-text-field>
            </v-container>
          </v-expand-transition>
        </v-form>

        <v-container class="mt-4">
          <v-btn text @click="stepperCurrentStep--">
            Back
          </v-btn>
          <v-btn color="primary" @click="stepperCurrentStep++">
            Continue
          </v-btn>
        </v-container>
      </v-stepper-content>

      <v-stepper-step :complete="stepperCurrentStep > 4" step="4" editable>
        Summary
      </v-stepper-step>

      <v-stepper-content step="4">
        <v-container>
          <v-row>framename / hostname: {{ frameName }}</v-row>
          <v-row>brightness: {{ brightness }}</v-row>
          <v-row>timezone: {{ timezone }}</v-row>
          <v-row>defaultMode: {{ defaultMode }}</v-row>
          <v-row>wifi: {{ selectedWifi ? selectedWifi.ssid : "none" }}</v-row>
        </v-container>

        <v-container class="mt-8">
          <v-btn color="primary" x-large>
            <v-icon left>
              mdi-rocket-launch
            </v-icon>
            Save and Launch
          </v-btn>
        </v-container>
      </v-stepper-content>
    </v-stepper>
  </v-container>
</template>

<script lang="ts">
import { orderBy } from "lodash";
import Vue from "vue";
import Component from "vue-class-component";
import { Wifi } from "@/models/environment";

@Component({
  components: {}
})
export default class ConfigurationWizardView extends Vue {
  public stepperCurrentStep = 1;
  public generalFormValid = false;
  public frameName = "pixelframe";
  public brightness = 128;
  public timezone = "Europe/Berlin";
  public defaultMode = "clock";
  public wifiFormValid = false;
  public selectedWifi: Wifi | null = null;
  public wifiPassword = "";
  public showWifiPassword = false;

  private availableWifis: Wifi[] = [
    {
      ssid: "Wizard test 1",
      signalStrength: 1
    },
    {
      ssid: "Cringe",
      signalStrength: 7
    },
    {
      ssid: "dgfhj",
      signalStrength: 3
    }
  ]; // FIXME: replace with data from service

  public get availableAndSortedWifis() {
    return orderBy(this.availableWifis, ["signalStrength"], "desc");
  }

  public async refreshWifiList(): Promise<void> {
    // TODO:
  }
}
</script>

<style>
.wifiIcon.selected {
  opacity: 1;
  color: var(--v-primary-base);
}

.wifiIcon {
  opacity: 0.4;
}
</style>
