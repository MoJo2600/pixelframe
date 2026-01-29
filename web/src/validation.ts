export const required = (v: string) => !!v

export const validPort = (v: string | number) => {
  const num = Number(v)
  return Number.isInteger(num) && num > 0 && num <= 65535
}
