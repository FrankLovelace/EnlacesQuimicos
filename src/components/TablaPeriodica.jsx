
import React from 'react';
import { elementos } from '../data/elementos';

const TablaPeriodica = ({ simbA, simbB, onTokenMove, onTokenReset }) => {

    const handleDragStart = (e, player) => {
        e.dataTransfer.setData("player", player);
        e.dataTransfer.effectAllowed = "move";
    };

    const handleDropOnCell = (e, targetSimb) => {
        e.preventDefault();
        const player = e.dataTransfer.getData("player");
        if (player) onTokenMove(player, targetSimb);
    };

    const handleDropOnNull = (e) => {
        e.preventDefault();
        const player = e.dataTransfer.getData("player");
        if (player) onTokenReset(player);
    };

    const handleDragOver = (e) => {
        e.preventDefault();
        e.dataTransfer.dropEffect = "move";
    };

    return (
        <div className="tabla-periodica">
            <div 
                className="null-zone"
                onDragOver={handleDragOver}
                onDrop={handleDropOnNull}
            >
                {!simbA && (
                    <div 
                        className="token token-e1" 
                        draggable 
                        onDragStart={(e) => handleDragStart(e, "A")}
                    >E1</div>
                )}
                {!simbB && (
                    <div 
                        className="token token-e2" 
                        draggable 
                        onDragStart={(e) => handleDragStart(e, "B")}
                    >E2</div>
                )}
                <span className="null-text">Null Zone</span>
            </div>

            {elementos.map((el) => (
                <div
                    key={el.simb}
                    className="celda-elemento"
                    style={{ gridColumn: el.col, gridRow: el.fila }}
                    onDragOver={handleDragOver}
                    onDrop={(e) => handleDropOnCell(e, el.simb)}
                >
                    {simbA === el.simb && (
                        <div 
                            className="token token-e1 positioned" 
                            draggable 
                            onDragStart={(e) => handleDragStart(e, "A")}
                        >E1</div>
                    )}
                    {simbB === el.simb && (
                        <div 
                            className="token token-e2 positioned" 
                            draggable 
                            onDragStart={(e) => handleDragStart(e, "B")}
                        >E2</div>
                    )}

                    <div className={`ficha-elemento bloque-${el.bloque}`}>
                        <span className="simbolo">{el.simb}</span>
                        <span className="nombre-corto">{el.nombre}</span>
                    </div>
                </div>
            ))}
        </div>
    );
};

export default TablaPeriodica;