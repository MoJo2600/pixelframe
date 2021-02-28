export const required = (v: string) => !!v;

export const validPort = (v: string | number) =>
  Number.isInteger(Number(v)) && v > 0 && v <= 65535;
