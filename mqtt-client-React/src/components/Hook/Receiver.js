import React, { useEffect, useState } from 'react';
import { Card, List } from 'antd';

const Receiver = ({ payload }) => {
  const [temperatura, setTemperatura] = useState({ title: "Temperatura", value: 0 })
  const [umidade, setUmidade] = useState({ title: "Umidade", value: 0 })
  const [histTemperatura, setHisTemperatura] = useState({ title: "Temperatura", values: [] })
  const [histUmidade, setHistUmidade] = useState({ title: "Umidade", values: [] })
  const [intervaloDeTempo, setIntervaloDeTempo] = useState({ title: "Intervalo de Tempo", value: 0 })
  const [data, setData] = useState([])
  const [historicos, setHistoricos] = useState([])

  useEffect(() => {
    if (payload.topic && payload.topic === 'tp_03_g04/intervalo') {
      setIntervaloDeTempo(interval => { return { ...interval, value: payload.message } })
    } else if (payload.topic && payload.topic === 'tp_03_g04/dados') {
      const dados = JSON.parse(payload.message)
      const { temperaturas, umidades } = dados
      const temps = temperaturas.map(temp => temp + ' ° C')
      const umids = umidades.map(umid => umid + ' %')
      setHisTemperatura(hist => { return { ...hist, values: temps } })
      setHistUmidade(hist => { return { ...hist, values: umids } })
      setTemperatura(temp => { return { ...temp, value: temps[0] } })
      setUmidade(umidade => { return { ...umidade, value: umids[0] } })
    }
  }, [payload.message, payload.topic])

  useEffect(() => {
    setData([temperatura, umidade, intervaloDeTempo])
    setHistoricos([histTemperatura, histUmidade])
  }, [temperatura, umidade, intervaloDeTempo, histTemperatura, histUmidade])

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
        title={item.title}
        description={item.values.join('; ')}
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
