/**
 * FocusSession entity (placeholder).
 * Será implementada cuando se agregue el módulo de Focus Session / Pomodoro.
 */
export interface FocusSession {
  id: string;
  taskId?: string;
  startedAt: string;
  endedAt?: string;
  duration: number; // minutos planificados
  completed: boolean;
  createdAt: string;
  updatedAt: string;
}

export const FOCUS_SESSION_REPOSITORY_PORT = 'FOCUS_SESSION_REPOSITORY_PORT';

export interface FocusSessionRepository {
  findById(id: string): Promise<FocusSession | null>;
  findActive(): Promise<FocusSession | null>;
  findByTaskId(taskId: string): Promise<FocusSession[]>;
  findByDateRange(start: string, end: string): Promise<FocusSession[]>;
  save(session: FocusSession): Promise<FocusSession>;
  update(id: string, data: Partial<Omit<FocusSession, 'id' | 'createdAt'>>): Promise<FocusSession | null>;
  delete(id: string): Promise<boolean>;
}
