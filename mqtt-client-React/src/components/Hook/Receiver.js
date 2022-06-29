import React, { useEffect, useState } from 'react';
import { Card, List } from 'antd';

const Receiver = ({ payload }) => {
  const [temperatura, setTemperatura] = useState({ title: "Temperatura", value: 0 })
  const [umidade, setUmidade] = useState({ title: "Umidade", value: 0 })
  const [histTemperatura, setHisTemperatura] = useState({ title: "Temperatura", values: [] })
  const [histUmidade, setHistUmidade] = useState({ title: "Umidade", values: [] })
  const [intervaloDeTempo, setIntervaloDeTempo] = useState({ title: "Intervalo de Tempo (em ms)", value: 0 })
  const [luminosidade, setLuminosidade] = useState({ title: "Luminosidade", value: 0 })
  const [pressao, setPressao] = useState({ title: "Pressao", value: 0 })
  const [histLuminosidade, setHisLuminosidade] = useState({ title: "Luminosidade", values: [] })
  const [histPressao, setHistPressao] = useState({ title: "Pressao", values: [] })
  const [data, setData] = useState([])
  const [historicos, setHistoricos] = useState([])

  useEffect(() => {
    if (payload.topic && payload.topic === 'tp_03_g04/intervalo') {
      let intervalo = payload.message;
      if (intervalo < 2000) {
        intervalo = 2000
      }
      setIntervaloDeTempo(interval => { return { ...interval, value: intervalo } })
    } else if (payload.topic && payload.topic === 'tp_03_g04/dados') {
      const dados = JSON.parse(payload.message)
      const { temperaturas, umidades, pressoes, luminosidades } = dados
      setHisTemperatura(hist => { return { ...hist, values: temperaturas } })
      setHistUmidade(hist => { return { ...hist, values: umidades } })
      setHisLuminosidade(hist => { return { ...hist, values: luminosidades } })
      setHistPressao(hist => { return { ...hist, values: pressoes } })

      setTemperatura(temp => { return { ...temp, value: temperaturas[9].split('-')[0] } })
      setUmidade(umidade => { return { ...umidade, value: umidades[9].split('-')[0] } })
      setLuminosidade(lum => { return { ...lum, value: luminosidades[9].split('-')[0] } })
      setPressao(pressao => { return { ...pressao, value: pressoes[9].split('-')[0] } })
    }
  }, [payload.message, payload.topic])

  useEffect(() => {
    setData([temperatura, umidade, pressao, luminosidade, intervaloDeTempo])
    setHistoricos([histTemperatura, histUmidade, histLuminosidade, histPressao])
  }, [temperatura, umidade, intervaloDeTempo, histTemperatura, histUmidade, pressao, luminosidade, histLuminosidade, histPressao])

  const renderDadosItem = (item) => (
    <List.Item>
      <List.Item.Meta
        title={item.title}
        description={item.value}
      />
    </List.Item>
  )

  const renderHistoricoItem = (item) => (
    <List.Item>
      <List.Item.Meta
        style={{ "white-space": "pre-line" }}
        title={item.title}
        description={item.values.join('\n')}
      />
    </List.Item>
  )

  return (
    <>
      <Card
        title="Dados Atuais"
      >
        <List
          size="small"
          bordered
          dataSource={data}
          renderItem={renderDadosItem}
        />
      </Card>
      <Card
        title="Histórico (Últimas 10 medições)"
      >
        <List
          size="small"
          bordered
          dataSource={historicos}
          renderItem={renderHistoricoItem}
        />
      </Card>
    </>

  );
}

export default Receiver;
