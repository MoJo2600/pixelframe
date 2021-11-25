<template>
  <v-container>
    <configuration-input-wrapper
      title="Default mode"
      skeletonType="input"
      :error="error"
      :loading="loading"
    >
      <v-autocomplete
        v-if="!loading && !error"
        v-model="defaultMode"
        :items="availableDefaultModes"
        outlined
        hide-details
        :rules="[required]"
      ></v-autocomplete>
    </configuration-input-wrapper>
  </v-container>
</template>

<script lang="ts">
import Vue from "vue";
import Component from "vue-class-component";
import { Prop, VModel } from "vue-property-decorator";
import ConfigurationInputWrapper from "@/components/ConfigurationInputWrapper.vue";
import { required } from "@/validation";

@Component({
  components: {
    ConfigurationInputWrapper
  }
})
export default class DefaultModeInput extends Vue {
  required = required;

  @VModel({ type: String })
  public defaultMode!: string;

  @Prop()
  public availableDefaultModes!: string[];

  @Prop()
  public loading!: boolean;

  @Prop()
  public error!: boolean;
}
</script>
