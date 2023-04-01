export const enum Role
{
    PASSIVE,     // Someone else has lease.
    ACTIVE       // Has lease.
}

export function roleToString(role: Role): string
{
    switch(role)
    {
        case Role.PASSIVE:
            return "passive";
        case Role.ACTIVE:
            return "active";
        default:
            throw new Error(`Unhandled role provided ${role}.`); 
    }
}