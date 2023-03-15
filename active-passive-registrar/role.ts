export const enum Role
{
    NONE = 0,     // No-one has lease.
    PASSIVE,  // Someone else has lease.
    ACTIVE    // Has lease.
}