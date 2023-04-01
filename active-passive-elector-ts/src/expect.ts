export enum Severity{
    WARNING = 0,
    ERROR
}
const expect = (variable: any, variableName: string, severity: Severity = Severity.ERROR): any=>
{
    /*
    let a: number | undefined = 3;
    a = undefined;
    expect(a, "a", Severity.WARNING).result == false ? console.log("here") : null;
    */

    let result: boolean = true;
    if(variable === undefined){
        const message = `Expected ${variableName} to be defined.`;
        if(severity === Severity.ERROR){ result = false; throw new Error(message); }
        if(severity === Severity.WARNING){ result = false; console.log(message); }
    }

    const expectations: any = {variable, variableName, result};

    const isType = (expectations: any, type: string)=>
    {
        if(typeof(expectations.variable) !== type)
        {
            const message = `Expected ${expectations.variableName} to be of type ${type}.`;
            if(severity === Severity.ERROR){ expectations.result = false; throw new Error(message); }
            if(severity === Severity.WARNING){ expectations.result = false; console.log(message); }
        }

        return expectations;
    }
    expectations.isType = isType.bind(null, expectations);

    const isNotNull = (expectations: any)=>
    {
        if(expectations.variable === null)
        {
            const message = `Expected ${expectations.variableName} to not be null.`;
            if(severity === Severity.ERROR){ expectations.result = false; throw new Error(message); }
            if(severity === Severity.WARNING){ expectations.result = false; console.log(message); }
        }

        return expectations;
    }
    expectations.isNotNull = isNotNull.bind(null, expectations);

    const isEqualTo = (expectations: any, value: any)=>
    {
        if(expectations.variable !== value)
        {
            const message = `Expected ${expectations.variableName} to be equal to ${value} but was ${expectations.variable}.`;
            if(severity === Severity.ERROR){ expectations.result = false; throw new Error(message); }
            if(severity === Severity.WARNING){ expectations.result = false; console.log(message); }
        }

        return expectations;
    }
    expectations.isEqualTo = isEqualTo.bind(null, expectations);

    const isGreaterThanOrEqualTo = (expectations: any, value: any)=>
    {
        if(expectations.variable < value)
        {
            const message = `Expected ${expectations.variableName} to be greater than or equal to ${value} but was ${expectations.variable}.`;
            if(severity === Severity.ERROR){ expectations.result = false; throw new Error(message); }
            if(severity === Severity.WARNING){ expectations.result = false; console.log(message); }
        }

        return expectations;
    }
    expectations.isGreaterThanOrEqualTo = isGreaterThanOrEqualTo.bind(null, expectations);
    expectations.isGTorEQ = isGreaterThanOrEqualTo.bind(null, expectations);

    const isGreaterThan = (expectations: any, value: any)=>
    {
        if(expectations.variable <= value)
        {
            const message = `Expected ${expectations.variableName} to be greater than ${value} but was ${expectations.variable}.`;
            if(severity === Severity.ERROR){ expectations.result = false; throw new Error(message); }
            if(severity === Severity.WARNING){ expectations.result = false; console.log(message); }
        }

        return expectations;
    }
    expectations.isGreaterThan = isGreaterThan.bind(null, expectations); 
    expectations.isGT = isGreaterThan.bind(null, expectations); 

    const isLessThanOrEqualTo = (expectations: any, value: any)=>
    {
        if(expectations.variable > value)
        {
            const message = `Expected ${expectations.variableName} to be less than or equal to ${value} but was ${expectations.variable}.`;
            if(severity === Severity.ERROR){ expectations.result = false; throw new Error(message); }
            if(severity === Severity.WARNING){ expectations.result = false; console.log(message); }
        }

        return expectations;
    }
    expectations.isLessThanOrEqualTo = isLessThanOrEqualTo.bind(null, expectations); 
    expectations.isLTorEQ = isLessThanOrEqualTo.bind(null, expectations); 

    const isLessThan = (expectations: any, value: any)=>
    {
        if(expectations.variable >= value)
        {
            const message = `Expected ${expectations.variableName} to be less than ${value} but was ${expectations.variable}.`;
            if(severity === Severity.ERROR){ expectations.result = false; throw new Error(message); }
            if(severity === Severity.WARNING){ expectations.result = false; console.log(message); }
        }

        return expectations;
    }
    expectations.isLessThan = isLessThan.bind(null, expectations); 
    expectations.isLT = isLessThan.bind(null, expectations); 

    return expectations;
}

export default expect;