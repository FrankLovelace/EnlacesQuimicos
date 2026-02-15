import React from 'react';

const ELEMENTOS_DISPONIBLES = [
    { simb: 'Fe', nombre: 'Hierro' },
    { simb: 'O', nombre: 'Oxígeno' },
    { simb: 'Na', nombre: 'Sodio' },
    { simb: 'Cl', nombre: 'Cloro' },
    { simb: 'Au', nombre: 'Oro' },
    { simb: 'Ag', nombre: 'Plata' }
];

const SelectorSmash = ({ onSelect }) => {
    
    const handleDragStart = (e, simb) => {
        e.dataTransfer.setData("simbolo", simb);
    };

    return (
        <div className="smash-container">
            <h3>Zona de Selección</h3>
            <div className="pool-fichas">
                {ELEMENTOS_DISPONIBLES.map(el => (
                    <div 
                        key={el.simb}
                        className="ficha-elemento"
                        draggable
                        onDragStart={(e) => handleDragStart(e, el.simb)}
                    >
                        {el.simb}
                    </div>
                ))}
            </div>
        </div>
    );
};

export default SelectorSmash;