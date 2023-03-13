const expect = (variable, variableName)=>
{
    if(variable === undefined)
    {
        throw new Error(`Expected ${variableName} to be defined.`)
    }

    const expectations = {variable, variableName};

    const isType = (expectations, type)=>
    {
        if(typeof(expectations.variable) !== type)
        {
            throw new Error(`Expected ${expectations.variableName} to be of type ${type}.`)
        }

        return expectations;
    }
    expectations.isType = isType.bind(null, expectations);

    const isNotNull = (expectations)=>
    {
        if(expectations.variable === null)
        {
            throw new Error(`Expected ${expectations.variableName} to not be null.`)
        }

        return expectations;
    }
    expectations.isNotNull = isNotNull.bind(null, expectations);

    const isEqualTo = (expectations, value)=>
    {
        if(expectations.variable !== value)
        {
            throw new Error(`Expected ${expectations.variableName} to be equal to ${value} but was ${expectations.variable}.`)
        }

        return expectations;
    }
    expectations.isEqualTo = isEqualTo.bind(null, expectations);

    const isGreaterThanOrEqualTo = (expectations, value)=>
    {
        if(expectations.variable < value)
        {
            throw new Error(`Expected ${expectations.variableName} to be greater than or equal to ${value} but was ${expectations.variable}.`)
        }

        return expectations;
    }
    expectations.isGreaterThanOrEqualTo = isGreaterThanOrEqualTo.bind(null, expectations);
    expectations.isGTorEQ = isGreaterThanOrEqualTo.bind(null, expectations);

    const isGreaterThan = (expectations, value)=>
    {
        if(expectations.variable <= value)
        {
            throw new Error(`Expected ${expectations.variableName} to be greater than ${value} but was ${expectations.variable}.`)
        }

        return expectations;
    }
    expectations.isGreaterThan = isGreaterThan.bind(null, expectations); 
    expectations.isGT = isGreaterThan.bind(null, expectations); 

    const isLessThanOrEqualTo = (expectations, value)=>
    {
        if(expectations.variable > value)
        {
            throw new Error(`Expected ${expectations.variableName} to be less than or equal to ${value} but was ${expectations.variable}.`)
        }

        return expectations;
    }
    expectations.isLessThanOrEqualTo = isLessThanOrEqualTo.bind(null, expectations); 
    expectations.isLTorEQ = isLessThanOrEqualTo.bind(null, expectations); 

    const isLessThan = (expectations, value)=>
    {
        if(expectations.variable >= value)
        {
            throw new Error(`Expected ${expectations.variableName} to be less than ${value} but was ${expectations.variable}.`)
        }

        return expectations;
    }
    expectations.isLessThan = isLessThan.bind(null, expectations); 
    expectations.isLT = isLessThan.bind(null, expectations); 

    return expectations;
}

export default expect;