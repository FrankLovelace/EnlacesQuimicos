import React, { useState, useEffect, useRef, useCallback } from 'react';
import TablaPeriodica from './TablaPeriodica';

const VAK_CONFIG = {
    minX: 1.0, maxX: 9.0,
    minY: 0.0, maxY: 9.0,
    padding: 50
};

const CalculadoraVak = () => {
    const canvasRef = useRef(null);
    const api = useRef({ info: null, calcular: null });

    const [inputs, setInputs] = useState({ simbA: '', eA: '', simbB: '', eB: '' });
    const [infoA, setInfoA] = useState('Esperando A...');
    const [infoB, setInfoB] = useState('Esperando B...');
    const [resDetalle, setResDetalle] = useState('');
    const [puntosHistoria, setPuntosHistoria] = useState([]);

    const mapX = (val, w) => {
        const innerW = w - (VAK_CONFIG.padding * 2);
        return VAK_CONFIG.padding + ((val - VAK_CONFIG.minX) / (VAK_CONFIG.maxX - VAK_CONFIG.minX)) * innerW;
    };

    const mapY = (val, h) => {
        const innerH = h - (VAK_CONFIG.padding * 2);
        return (h - VAK_CONFIG.padding) - ((val - VAK_CONFIG.minY) / (VAK_CONFIG.maxY - VAK_CONFIG.minY)) * innerH;
    };

    const dibujarTrianguloBase = useCallback(() => {
        const cvs = canvasRef.current;
        if (!cvs) return;
        const ctx = cvs.getContext('2d');
        const { width: w, height: h } = cvs;

        ctx.clearRect(0, 0, w, h);

        const y5 = mapY(5.0, h);
        const y15 = mapY(1.5, h);
        const x6 = mapX(6.0, w);
        const xMin = mapX(VAK_CONFIG.minX, w);
        const xMax = mapX(VAK_CONFIG.maxX, w);

        ctx.fillStyle = "rgba(255, 255, 0, 0.1)";
        ctx.fillRect(xMin, mapY(VAK_CONFIG.maxY, h), xMax - xMin, y5 - mapY(VAK_CONFIG.maxY, h));
        ctx.fillStyle = "rgba(0, 200, 255, 0.1)";
        ctx.fillRect(xMin, y5, xMax - xMin, y15 - y5);
        ctx.fillStyle = "rgba(255, 50, 50, 0.15)";
        ctx.fillRect(xMin, y15, x6 - xMin, mapY(VAK_CONFIG.minY, h) - y15);
        ctx.fillStyle = "rgba(50, 255, 50, 0.1)";
        ctx.fillRect(x6, y15, xMax - x6, mapY(VAK_CONFIG.minY, h) - y15);

        ctx.strokeStyle = "#444";
        ctx.lineWidth = 1;
        ctx.beginPath();
        ctx.moveTo(xMin, y5); ctx.lineTo(xMax, y5);
        ctx.moveTo(xMin, y15); ctx.lineTo(xMax, y15);
        ctx.moveTo(x6, y15); ctx.lineTo(x6, mapY(0, h));
        ctx.stroke();

        ctx.strokeStyle = "#fff";
        ctx.lineWidth = 2;
        ctx.beginPath();
        ctx.moveTo(mapX(VAK_CONFIG.minX, w), mapY(0, h)); ctx.lineTo(mapX(VAK_CONFIG.maxX, w), mapY(0, h));
        ctx.moveTo(mapX(VAK_CONFIG.minX, w), mapY(0, h)); ctx.lineTo(mapX(VAK_CONFIG.minX, w), mapY(VAK_CONFIG.maxY, h));
        ctx.stroke();

        puntosHistoria.forEach(p => {
            const px = mapX(p.x, w);
            const py = mapY(p.y, h);
            ctx.beginPath();
            ctx.arc(px, py, 7, 0, 2 * Math.PI);
            ctx.fillStyle = "#fff";
            ctx.fill();
            ctx.strokeStyle = "#000";
            ctx.stroke();
            ctx.fillStyle = "#eee";
            ctx.font = "bold 12px Arial";
            ctx.fillText(p.label, px + 10, py + 4);
        });
    }, [puntosHistoria]);

    const obtenerInfoWasm = useCallback((id, simb, elec) => {
        if (!api.current.info) return;
        if (!simb) {
            id === 'A' ? setInfoA('Esperando A...') : setInfoB('Esperando B...');
            return;
        }
        try {
            const res = JSON.parse(api.current.info(simb, parseInt(elec || 0)));
            const targetSet = id === 'A' ? setInfoA : setInfoB;
            if (!res.error) {
                let signo = res.carga > 0 ? ">" : (res.carga < 0 ? "<" : "=");
                targetSet(`<b>${res.nombre}</b> (${res.carga > 0 ? '+' : ''}${res.carga})<br>Mul: ${res.mulliken.toFixed(2)} eV<br>Pauling ${signo} ${res.pauling_ref.toFixed(2)}`);
            } else {
                targetSet(`<span style="color:darkgray">Desconocido</span>`);
            }
        } catch (e) { console.error(e); }
    }, []);

    const calcular = () => {
        if (!api.current.calcular) return;
        const data = JSON.parse(api.current.calcular(inputs.simbA, parseInt(inputs.eA || 0), inputs.simbB, parseInt(inputs.eB || 0)));
        if (data.error) { alert(data.error); return; }
        setResDetalle(`<span><b>${data.tipo}</b></span> <span>k=${data.k}</span>`);
        setPuntosHistoria(prev => [...prev, { x: data.vak_x, y: data.vak_y, label: `${inputs.simbA}-${inputs.simbB}` }]);
    };

    useEffect(() => {
        if (window.createModule) {
            window.createModule().then(Module => {
                api.current.info = Module.cwrap('obtener_info_elemento_wasm', 'string', ['string', 'number']);
                api.current.calcular = Module.cwrap('calcular_enlace_wasm', 'string', ['string', 'number', 'string', 'number']);
                dibujarTrianguloBase();
            });
        }
    }, [dibujarTrianguloBase]);

    useEffect(() => {
        dibujarTrianguloBase();
    }, [puntosHistoria, dibujarTrianguloBase]);

    const handleInputChange = (e) => {
        const { id, value } = e.target;
        const nuevosInputs = { ...inputs, [id]: value };
        setInputs(nuevosInputs);
        const tag = id.includes('A') ? 'A' : 'B';
        obtenerInfoWasm(tag, nuevosInputs[`simb${tag}`], nuevosInputs[`e${tag}`]);
    };

    const handleTokenMove = (player, targetSimb) => {
        const nuevosInputs = { ...inputs, [`simb${player}`]: targetSimb };
        setInputs(nuevosInputs);
        obtenerInfoWasm(player, targetSimb, nuevosInputs[`e${player}`]);
    };

    const handleTokenReset = (player) => {
        const nuevosInputs = { ...inputs, [`simb${player}`]: '' };
        setInputs(nuevosInputs);
        obtenerInfoWasm(player, '', nuevosInputs[`e${player}`]);
    };

    return (
        <div className="dashboard">
            <div className="left-panel">
                <div className="canvas-container">
                    <canvas ref={canvasRef} width="800" height="500"></canvas>
                </div>
                <TablaPeriodica 
                    simbA={inputs.simbA} 
                    simbB={inputs.simbB} 
                    onTokenMove={handleTokenMove}
                    onTokenReset={handleTokenReset}
                />
            </div>

            <div className="right-panel">
                <div className="main-panel">
                    <h2 style={{ marginTop: 0, color: 'var(--primary)' }}>Configuración</h2>
                    <div className="atom-row">
                        <div className="input-group">
                            <label>Atómo A</label>
                            <input type="text" id="simbA" className="inp-simb" value={inputs.simbA} onChange={handleInputChange} />
                        </div>
                        <div className="input-group">
                            <label>Electrones</label>
                            <input type="number" id="eA" className="inp-elec" value={inputs.eA} onChange={handleInputChange} />
                        </div>
                    </div>
                    <div className="atom-row">
                        <div className="input-group">
                            <label>Atómo B</label>
                            <input type="text" id="simbB" className="inp-simb" value={inputs.simbB} onChange={handleInputChange} />
                        </div>
                        <div className="input-group">
                            <label>Electrones</label>
                            <input type="number" id="eB" className="inp-elec" value={inputs.eB} onChange={handleInputChange} />
                        </div>
                    </div>
                    <button onClick={calcular}>CALCULAR ENLACE</button>
                    <div className="output-grid" dangerouslySetInnerHTML={{ __html: resDetalle }}></div>
                </div>

                <div className="stats-panel">
                    <h3 style={{ marginTop: 0 }}>Resultados Atómicos</h3>
                    <div style={{ marginBottom: '10px', borderBottom: '1px solid #444', paddingBottom: '10px' }} dangerouslySetInnerHTML={{ __html: infoA }}></div>
                    <div dangerouslySetInnerHTML={{ __html: infoB }}></div>
                </div>
            </div>
        </div>
    );
};

export default CalculadoraVak;