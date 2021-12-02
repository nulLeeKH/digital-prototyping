from fastapi import FastAPI
import uvicorn


app = FastAPI()


@app.get("/api/upload/status/{device_id}")
async def get_status(device_id: int, a: float, v: float, b: int):
    print(device_id, a, v, b)
    return


if __name__ == "__main__":
    uvicorn.run("main:app", host="0.0.0.0", port=31372, reload=True)

