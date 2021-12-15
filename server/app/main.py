from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
import uvicorn


app = FastAPI()


app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


data = [
    {
    'vol': 0,
    'acc': 0,
    'btn': False,
    },
]


@app.get("/api/upload/status/{device_id}")
async def get_status(device_id: int, a: float, v: float, b: int):
    print(device_id, a, v, b)
    data[device_id]['vol'] = a
    data[device_id]['acc'] = abs(v - 1)
    if b:
        data[device_id]['btn'] = not data[device_id]['btn']
    return


@app.get("/api/download/status/{device_id}")
async def get_status(device_id: int):
    return data[device_id]


if __name__ == "__main__":
    uvicorn.run("main:app", host="0.0.0.0", port=31372, reload=True)

