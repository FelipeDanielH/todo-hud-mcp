export interface BaseRepository<T> {
  findAll(): Promise<T[]>;
  findById(id: string): Promise<T | null>;
  save(entity: T): Promise<T>;
  update(id: string, data: Partial<Omit<T, 'id' | 'createdAt'>>): Promise<T | null>;
  delete(id: string): Promise<boolean>;
}
