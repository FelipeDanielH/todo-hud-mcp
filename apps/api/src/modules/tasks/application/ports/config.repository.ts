/**
 * ConfigEntry representa una clave/valor genérico de configuración de usuario.
 * Será expandida cuando se implemente el módulo de configuración.
 */
export interface ConfigEntry {
  key: string;
  value: string;
  updatedAt: string;
}

export const CONFIG_REPOSITORY_PORT = 'CONFIG_REPOSITORY_PORT';

export interface ConfigRepository {
  get(key: string): Promise<string | null>;
  set(key: string, value: string): Promise<void>;
  getAll(): Promise<ConfigEntry[]>;
  delete(key: string): Promise<boolean>;
}
