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
          <v-row class="pt-8" style="max-width: 320px;">
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

      <v-stepper-step
        :complete="stepperCurrentStep > 2"
        step="2"
        editable
        :rules="[() => generalFormValid]"
      >
        General
      </v-stepper-step>

      <v-stepper-content step="2">
        <v-form v-model="generalFormValid">
          <frame-name-input
            v-model="frameName"
            :error="false"
            :loading="false"
          />
          <timezone-input v-model="timezone" :error="false" :loading="false" />
          <default-mode-input
            v-model="defaultMode"
            :availableDefaultModes="availableDefaultModes"
            :error="false"
            :loading="false"
          />
          <brightness-input
            v-model="brightness"
            :error="false"
            :loading="false"
          />
        </v-form>

        <v-container class="mt-4">
          <v-btn text @click="stepperCurrentStep--">
            Back
          </v-btn>
          <v-btn
            color="primary"
            @click="stepperCurrentStep++"
            :disabled="!generalFormValid"
          >
            Continue
          </v-btn>
        </v-container>
      </v-stepper-content>

      <v-stepper-step
        :complete="stepperCurrentStep > 3"
        step="3"
        editable
        :rules="[() => wifiFormValid]"
      >
        WiFi
      </v-stepper-step>

      <v-stepper-content step="3">
        <v-form v-model="wifiFormValid">
          <wifi-input
            v-model="selectedWifi"
            :availableWifis="availableWifis"
            :error="false"
            :loading="false"
            @onRefreshClick="refreshWifiList"
          />

          <wifi-password-input
            v-model="wifiPassword"
            :error="false"
            :loading="false"
            :show="selectedWifi !== null"
          />
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
          <p v-if="!generalFormValid || !wifiFormValid" class="red--text">
            There is at least one error in the configuration sections. Please
            have a look at the highlighted sections of the wizard and fix the
            errors or fill in the required data.
          </p>
          <v-btn
            color="primary"
            x-large
            :disabled="!generalFormValid || !wifiFormValid"
          >
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
import Vue from "vue";
import Component from "vue-class-component";
import { Wifi } from "@/models/environment";

import FrameNameInput from "@/components/inputs/FrameName.vue";
import TimezoneInput from "@/components/inputs/Timezone.vue";
import DefaultModeInput from "@/components/inputs/DefaultMode.vue";
import BrightnessInput from "@/components/inputs/Brightness.vue";
import WifiInput from "@/components/inputs/Wifi.vue";
import WifiPasswordInput from "@/components/inputs/WifiPassword.vue";

@Component({
  components: {
    FrameNameInput,
    TimezoneInput,
    DefaultModeInput,
    BrightnessInput,
    WifiInput,
    WifiPasswordInput
  }
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

  public get availableDefaultModes() {
    return ["clock", "gif"];
  }

  public availableWifis: Wifi[] = [
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

  public async refreshWifiList(): Promise<void> {
    // TODO:
    console.log("refresh");
  }
}
</script>
